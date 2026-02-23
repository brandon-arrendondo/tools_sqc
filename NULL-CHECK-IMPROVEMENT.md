# EXP34-C / CWE-476 Improvement Roadmap: 80%+ TP Rate

**Goal**: Raise CWE-476 (NULL Pointer Dereference) TP rate from 35.4% → 80%+
**Date**: 2026-02-23
**Baseline** (Round 15, MCP): 357 TP, 651 FP, 35.4% TP rate

---

## 1. The Hard Constraint: 80% Requires Both Axes

The core obstacle is mathematical, not just algorithmic.

### Current CWE-476 violation breakdown

| Source | TP | FP | TP Rate |
|--------|---:|---:|--------:|
| EXP34-C | 82 | 50 | 62.1% |
| MEM10-C | 28 | 134 | 17.3% |
| EXP33-C | 28 | 94 | 22.9% |
| DCL13-C | 39 | 83 | 32.0% |
| API02-C | 37 | 76 | 32.7% |
| API00-C | 39 | 65 | 37.5% |
| Other | 104 | 149 | 41.1% |
| **Total** | **357** | **651** | **35.4%** |

EXP34-C detects only **82 of 1,098 FLAW lines** (7.5% flaw-line detection rate). The remaining 275 TPs come from collateral rules firing on bad() code structure.

### The ceiling problem

If EXP34-C perfectly detected all 1,098 FLAW lines (+1,016 TPs) with zero new FPs:
```
EXP34-C:           1,098 TP,   50 FP
Collateral rules:    275 TP,  601 FP
Total:             1,373 TP,  651 FP → 67.8%
```

**Still not 80%.** The 601 collateral FPs are a structural ceiling. For 80% with 1,373 TPs, FP must drop to ≤343 — requiring ~258 FP eliminated from collateral rules.

**Conclusion**: 80% requires BOTH more EXP34-C TPs AND reduced collateral FPs.

---

## 2. Juliet CWE-476 Variant Taxonomy

The 372 files (across ~6 data types) cover ~62 distinct code patterns:

| Tier | Variants | Files | Pattern | Achievable? |
|------|----------|------:|---------|-------------|
| **A** | 01-10, 21-22 | ~72 | Direct NULL assign + deref; constant conditions (if(1), if(0)) | ✅ With CFG dataflow |
| **B** | 31-34 | ~24 | Copy chains, pointer-to-pointer aliasing, union aliasing | ✅ Partial (copies yes; union aliasing hard) |
| **C** | 44-45 | ~12 | Function pointer calls; static global null across same-file functions | ✅ With global pre-pass |
| **D** | 51-58 | ~120 | Cross-file parameter passing (null passed to sink in different .c file) | ✅ With call-site null propagation |
| **E** | 63-68 | ~72 | Pointer-to-pointer and global cross-file | ⚠️ Hard (whole-program alias needed) |
| **F** | 72-82 | ~72 | C++/Assembly constructs | ❌ Out of scope |

### Key patterns (from actual Juliet files)

**Variant 01 (direct)** — already partially caught:
```c
// bad(): data = NULL; printHexCharLine(data[0]);
// goodB2G(): data = NULL; if (data != NULL) { data[0]; }
// goodG2B(): data = "Good"; data[0];
```

**Variant 12 (opaque branch)** — fixed in Round 15:
```c
// bad(): if(globalReturnsTrueOrFalse()) { data=NULL; } else { data="Good"; }
//         if(globalReturnsTrueOrFalse()) { data[0]; }   ← FLAW (no null check)
// goodB2G(): same but with if(data!=NULL) guard before dereference
```

**Variant 31 (copy chain)**:
```c
// bad(): data = NULL; { char *dataCopy = data; char *data = dataCopy; data[0]; }
// Copy propagation: data→dataCopy→inner data → all potentially null
```

**Variant 45 (static global)**:
```c
// bad(): static char *g; ... g = NULL; badSink(); void badSink() { g[0]; }
// Requires pre-pass over static globals in same TU
```

**Variant 51 (cross-file)**:
```c
// 51a.c: data = NULL; badSink(data);
// 51b.c: void badSink(char *data) { data[0]; }  ← FLAW inside #ifndef OMITBAD
// 51b.c: void goodSink(char *data) { if(data!=NULL){ data[0]; } }
```

Note: sink file functions ARE inside `#ifndef OMITBAD`/`#ifndef OMITGOOD` guards, so violations there count toward benchmark TP/FP.

---

## 3. Three Pillars to 80%

### Pillar 1 — CFG-based Null State Dataflow (EXP34-C overhaul)

**Replace** the current linear-walk `collect_null_variables` with forward dataflow on the existing CFG infrastructure.

#### Null lattice
```
DefinitelyNull → PossiblyNull ← NotNull
                     ↑
                  Unknown
```

Join semantics (merge points):
- `DefinitelyNull ⊔ NotNull = PossiblyNull`
- `PossiblyNull ⊔ anything = PossiblyNull`
- `Unknown ⊔ x = x` (or `PossiblyNull` for conservative analysis)

#### Transfer functions
```
ptr = NULL                →  state[ptr] = DefinitelyNull
ptr = malloc/calloc(...)  →  state[ptr] = PossiblyNull
ptr = "literal" / &var    →  state[ptr] = NotNull
ptr = other_var           →  state[ptr] = state[other_var]
ptr = func()              →  state[ptr] = summary.null_return[func] || PossiblyNull
if (ptr == NULL) { ... }  →  true-edge: DefinitelyNull; false-edge: NotNull
if (ptr != NULL) { ... }  →  true-edge: NotNull; false-edge: DefinitelyNull
if (!ptr) return;         →  after: NotNull
assert(ptr != NULL)       →  after: NotNull
```

#### What this handles that current code cannot

| Pattern | Current | CFG |
|---------|:-------:|:---:|
| Constant conditions (if(1)) | ✅ | ✅ |
| if/else branch merge | ✅ R15 | ✅ |
| Loops (while/for back-edges) | ❌ | ✅ |
| Early-return guard: `if(!p) return;` | ✅ | ✅ |
| Nested conditions | ⚠️ partial | ✅ |
| Ternary: `p = cond ? NULL : valid` | ❌ | ✅ |
| Switch case null flow | ❌ | ✅ |
| FP: null check dominates ALL paths to deref | ⚠️ partial | ✅ |

#### Static global pre-pass (for Variant 45)
Before analyzing any function, scan all static/global variable initializers and assignments in the TU. Build `global_null_state: HashMap<String, NullValue>`. Initialize function-level state from this map so functions that read globals start with the correct null state.

**Estimated gain**: Tiers A-C = ~108 files. At 85% detection: ~275 additional EXP34-C TPs.

---

### Pillar 2 — Call-Site Null Propagation (Tier D, cross-file parameters)

Cross-file variants (51-68) have the null assignment in file A and the dereference in file B. The sink file B already runs with all pointer parameters as `PossiblyNull` (current behavior). The gap is the **source file A**: passing a definitely-null pointer to a function is not currently flagged.

#### Approach 2a: Call-site flagging
When a call passes a definitely/possibly-null value as a pointer argument, flag EXP34-C at the call site. This catches the source (A-file) side:
```c
// 51a.c: data = NULL; badSink(data);  ← flag here: null passed to function
```

#### Approach 2b: Parameter null inference (Tier D sink side)
Extend `FunctionSummary` with `possibly_null_params: Vec<usize>`. In the two-pass directory scan:
1. **Pass 1**: For each call site where null/possibly-null is passed to parameter `i`, record in callee's summary.
2. **Pass 2**: When analyzing callee, if `possibly_null_params[i]`, initialize that param as `PossiblyNull`.

This gives the sink function accurate null state for its parameter even without seeing the caller.

The good-function counterparts (`goodB2GSink`) always add `if (data != NULL)` guards, so no new FPs are expected.

**Estimated gain**: Tier D = ~120 files. At 70% detection: ~252 additional EXP34-C TPs.

---

### Pillar 3 — Collateral Rule FP Reduction

The 601 non-EXP34-C FPs in CWE-476 have specific, fixable causes:

| Rule | FP Count | Root Cause | Proposed Fix | Est. FP Reduction |
|------|:--------:|------------|--------------|:-----------------:|
| **MEM10-C** | 134 → **28** ✅ | Fires on `if (data != NULL)` in good() — penalizes inline null checks as "should use validation function." Good functions fix the vuln by ADDING null checks, which MEM10-C then flags. | ✅ DONE R16: Only fire when the inline null check is for a **function parameter**, not a locally-declared variable. Verified: 134→28 FPs (106 reduction). Remaining 28 FPs are from goodB2GSink-style functions where `data` IS a param — unavoidable without call-site analysis. | **106** |
| **EXP33-C** | 94 | Fires on variables that appear uninitialized in some branches; good() functions add branches (null checks) that confuse initialization tracking. | Apply CFG-based branch merge to EXP33-C initialization tracking (same fix as Pillar 1). | ~60 |
| **DCL13-C** | 83 | Fires on sink functions in OMITGOOD sections (`goodG2BSink(char *data)` — `data` not modified, should be const). The main() skip in R16 had no benchmark impact: main() is in `#ifdef INCLUDEMAIN`, not OMITBAD/OMITGOOD. Real FPs come from functions with identical signatures in both bad/good sections. | No clean fix — same structure in bad() and good() sections means any suppression removes TPs equally. Accept as structural. | ~0 |
| **API02-C** | 76 | Fires on function signatures; structural noise in both bad() and good() sections. | Narrow scope or accept as structural. | ~30 |
| **API00-C** | 65 | Same as API02-C. | Same. | ~20 |

**Total estimated FP reduction: ~176 FPs** → collateral FPs drop from 601 to ~425 (MEM10-C gave 106, others remain structural).

---

## 4. Expected Outcomes Per Phase

| Phase | Work | EXP34-C TP | Collateral FP | Est. CWE-476 TP Rate |
|-------|------|:----------:|:-------------:|:--------------------:|
| Baseline (R15) | — | 82 | 601 | 35.4% |
| **Phase 1** | CFG dataflow (Tiers A-C) + DCL13-C/MEM10-C fixes | ~350 | ~480 | ~48-52% |
| **Phase 2** | Call-site propagation (Tier D) + EXP33-C fix | ~600 | ~400 | ~60-65% |
| **Phase 3** | Tier E coverage + API rule narrowing | ~750 | ~300 | ~70-75% |
| **Phase 4** | Remaining FP reduction + edge cases | ~850 | ~200 | **80-85%** |

---

## 5. Implementation Plan

### Phase 1 Tasks (immediate, 4-8 weeks)

1. **New module**: `src/analyze/null_state.rs`
   - `NullLattice` enum: `DefinitelyNull`, `PossiblyNull`, `NotNull`, `Unknown`
   - `NullState: HashMap<String, NullLattice>`
   - `join_states(a: &NullState, b: &NullState) -> NullState`
   - `transfer(stmt: &Node, state: &NullState, source: &str) -> NullState`

2. **Refactor EXP34-C**: Replace `collect_null_variables` + `is_unsafe_dereference` with `NullStateAnalyzer` driven by `cfg.rs` fixpoint iteration.

3. **Global pre-pass**: Static/global variable null assignment tracking for same-TU cross-function flow (Variant 45).

4. **DCL13-C fix**: Skip `main()` parameters (quick win, ~30 FP reduction in CWE-476).

5. **MEM10-C fix**: Restrict to parameter null checks only (quick win, ~110 FP reduction in CWE-476).

### Phase 2 Tasks (4-8 weeks after Phase 1)

6. **Extend `FunctionSummary`**: Add `possibly_null_params` field.

7. **Two-pass analysis**: First pass collects call-site null info; second pass uses it in callee analysis.

8. **EXP33-C CFG integration**: Apply branch-merge fix from Pillar 1 to EXP33-C initialization tracking.

### Phase 3+ Tasks (ongoing)

9. **Tier E (global cross-file)**: Whole-program global null tracking (hard; requires multi-TU coordination).

10. **API00-C / API02-C scope narrowing**: Investigate whether these rules can be tightened without losing global TPs.

---

## 6. Key Risks

| Risk | Likelihood | Impact | Mitigation |
|------|:----------:|:------:|------------|
| CFG dataflow adds FPs in good() functions | Medium | High | Strong lattice semantics; validate per-CWE after each change |
| Call-site propagation inflates EXP34-C globally | High | High | Gate on explicit null evidence; not "all pointer params are null" |
| MEM10-C fix loses significant global TPs | Medium | Medium | Run full benchmark after; MEM10-C TP:FP ratio in CWE-476 is 28:134, bad enough to justify even at cost of some global TPs |
| 80% requires API00-C/API02-C changes too | Medium | Medium | These are low-TP rules; scoping them is lower risk |
| Tier E is disproportionately hard | High | Low | Accept as known gap; 75% is achievable without it |

---

## 7. What NOT to Do

- **Do not** flag all pointer parameter dereferences unconditionally (current behavior for uninitialized params). This causes massive FPs on real-world code. The call-site propagation approach (Pillar 2) is the correct alternative.
- **Do not** disable collateral rules (MEM10-C, EXP33-C) outright — they have real TPs globally. Fix them surgically.
- **Do not** use OMITBAD/OMITGOOD file context to suppress rules — this would be benchmark-tuning, not real improvement.
- **Do not** conflate FLAW-line detection rate (currently 0%) with TP rate. The benchmark TP rate counts all violations in OMITBAD sections, not just at the exact FLAW comment line.

---

## 8. Progress Tracking

| Round | CWE-476 TP Rate | EXP34-C TPs | Collateral FPs | Notes |
|-------|:--------------:|:-----------:|:--------------:|-------|
| 13 (legacy) | ~33% | ~80 | ~600 | Pre-deref_after_check |
| 14 | +18 EXP34-C TP | 100 | ~600 | deref_after_check pattern |
| 15 | 35.4% | 82 | 601 | if/else branch merge (variant 12) |
| 16 | ~37-40%? | 82 | ~495 | MEM10-C param-only fix (−106 FP); DCL13-C main() no benchmark impact; pending full run |
| 17 (target) | ~50% | ~350 | ~480 | CFG dataflow + remaining FP fixes |
| 17 (target) | ~65% | ~600 | ~400 | Call-site propagation + EXP33-C fix |
| 18+ (target) | **≥80%** | **~850** | **~200** | Full architecture |
