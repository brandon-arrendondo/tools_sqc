# Claude Code Project Instructions

## /work-active Workflow (CONDITIONAL)

**ONLY follow this workflow IF:**
- Current git branch matches pattern `claude-work-active-*` (check with `git branch --show-current`)
- OR you're explicitly working on proposals from `AGENTS/PROPOSALS/ACTIVE/`
- OR the user invoked `/work-active` earlier in this session

**If NOT in a /work-active session, ignore this section.**

When working on proposals from `/work-active` command (implementing CERT C rules), **ALWAYS** use the helper script workflow:

### Setup (Run Once Per Session)
```bash
scripts/work_active_helpers.sh verify-precommit
scripts/work_active_helpers.sh list-proposals SUBDIR
```

**IMPORTANT: On every continuation (especially after context compaction):**
- ALWAYS re-read your current active proposal before continuing work
- Verify you're following the Implementation Constraints section
- Check Implementation Log to see what's already been completed

```bash
# Re-read current proposal (replace with actual file path)
cat AGENTS/PROPOSALS/ACTIVE/{SELECTED_SUBDIR}/{CURRENT_PROPOSAL}.md
```

### For EACH Rule Implementation
```bash
# 1. Lock all files except the specific rule you're implementing
scripts/work_active_helpers.sh lock-for-impl RULE_ID

# Alternative: Manual mode (specify exact files)
# scripts/work_active_helpers.sh lock-except \
#   "src/rules/cert_c/CATEGORY/RULE_ID/rule_id_c.rs" \
#   "src/rules/cert_c/CATEGORY/RULE_ID/RULE_ID.toml"

# 2. Implement rule (only unlocked files are writable)
# - Create src/rules/cert_c/CATEGORY/RULE_ID/rule_id_c.rs
# - IMPORTANT: Do NOT add embedded unit tests (no #[cfg(test)] modules)
# - Test cases come from .c files in tests/ directory (auto-generated)
# - Test files are LOCKED (chmod 000) - read test examples from proposal markdown

# 3. Unlock before registration in mod.rs
scripts/work_active_helpers.sh unlock-all

# 4. Register in mod.rs and enable in TOML
# 5. Build and test
# 6. Commit and move proposal to STAGED
```

### Implementation Rules (CRITICAL)

**NEVER add embedded unit tests in rule implementation files:**
- ❌ NO `#[cfg(test)]` modules in `src/rules/cert_c/*/*/*.rs` files
- ❌ NO inline test functions with hardcoded C code snippets
- ✅ Test cases come from `.c` files in `tests/` directory (auto-generated into Rust tests)
- ✅ If no test cases exist for a rule, implement WITHOUT tests (this is acceptable)

**Why:**
- Embedded tests are redundant (same stub pattern with different hardcoded C)
- Poor separation of responsibilities (implementation vs. testing)
- Test infrastructure auto-generates tests from `.c` files

### Key Commands
- `lock-for-impl RULE_ID` - Lock all except rule implementation (tests LOCKED)
- `lock-for-test RULE_ID` - Lock all except rule test files (impl LOCKED)
- `lock-except FILE1 FILE2...` - Manual mode: lock all except specified files
- `unlock-all` - Restore write permissions in configured dirs
- `extract-rule-id FILE` - Get rule ID from proposal filename

### Lock Configuration
- Lock scope configured in `.claude/lock-list.yaml`
- Default: locks all files in `src/` directory
- Exclusions: `.git/`, `target/`, `tmp/`, `scripts/`
- Test files are LOCKED during implementation (chmod 000 - no read or write)
- Get test case context from proposal markdown, NOT from locked test files

### Why This Matters
- Prevents accidental modifications to test files and shared infrastructure
- Enforces single-rule focus through file-level access control
- Maintains code isolation during development
- chmod 000 protection blocks even AI tools from modifying locked files

**If this workflow wasn't followed earlier in the session, start following it NOW.**

---

## /gather-opinions Workflow (CONDITIONAL)

**ONLY follow this workflow IF:**
- Current git branch matches pattern `opinions/*` (check with `git branch --show-current`)
- OR the user invoked `/gather-opinions` earlier in this session

**If NOT in a /gather-opinions session, ignore this section.**

### Branch Name Format
```
opinions/{persona-slug}-{reviewer}-{date}
```

### On Every Continuation (CRITICAL - After Context Compaction)

**Step 1: Detect workflow from branch name**
```bash
git branch --show-current
# If matches opinions/*, you are in /gather-opinions workflow
```

**Step 2: Extract persona from branch and re-read persona file**
```bash
# Parse persona slug from branch name (between opinions/ and reviewer name)
# Then read the matching persona file from AGENTS/PERSONAS/
cat AGENTS/PERSONAS/{matching-persona}.md
```

**Step 3: Re-read the workflow definition (source of truth)**
```bash
cat .claude/commands/gather-opinions.md
```

**Step 4: Check TodoWrite state for current progress**
- The TodoWrite tool persists across compaction
- Expect exactly 6 todos:

  *Workflow steps for current proposal:*
  1. "LOCK + Read proposal {PROPOSAL}" - `in_progress` or `completed`
  2. "Read ALL related_files for {RULE_ID}" - `pending`, `in_progress`, or `completed`
  3. "Form opinion on {PROPOSAL}" - `pending`, `in_progress`, or `completed`
  4. "Record + commit + UNLOCK {PROPOSAL}" - `pending` or `in_progress`

  *Tracking:*
  5. "Next: {NEXT_PROPOSAL}" - `pending`
  6. "Progress: N/TOTAL reviewed" - `in_progress`

- **⚠️ Do NOT modify this structure or batch proposals** - it enables recovery
- Resume from whichever workflow step (1-4) is `in_progress`
- If todos have deviated from this structure, reset to correct format before continuing
- **CRITICAL:** Step 2 requires reading ALL files in `related_files` frontmatter - no skipping

**Anti-pattern to watch for:** If Claude starts batching proposals or modifying the 6-todo structure for "efficiency," this BREAKS the workflow. The number of proposals is irrelevant - never batch, regardless of scale.

**Step 5: Continue the workflow as defined in gather-opinions.md**

### Why This Matters

- Branch name encoding enables workflow recovery after compaction
- Re-reading persona file ensures consistent analysis perspective
- gather-opinions.md is the single source of truth for workflow steps
- TodoWrite tracking ensures no proposals are skipped across compactions

**If this workflow wasn't followed earlier in the session, start following it NOW.**

---

## Benchmark Workflow (CRITICAL)

When running Juliet benchmarks, follow this protocol strictly:

1. **Version bump + commit BEFORE benchmark**: Always bump the version in `Cargo.toml`,
   rebuild (`cargo build --release`), and commit before starting a benchmark run.
   This ensures the benchmark results directory is tagged with the correct version
   and commit SHA (e.g., `sqc-0.2.1-abc1234`).

2. **NEVER modify code while a benchmark is running**: The benchmark uses
   `target/release/sqc`. If you rebuild while it's running, you corrupt results
   mid-run. Make ALL code changes and commits BEFORE starting the benchmark.

3. **Wait for completion**: Benchmarks take ~40-50 minutes. The last CWE category
   takes the longest. Check status with `get_status()` no more than once every
   10 minutes. Do NOT make changes or start other work until the benchmark completes.

4. **Compare runs**: After a benchmark completes, use `compare_runs()` to compare
   against previous runs. Use `get_cwe_detail()` for per-CWE deep dives.

5. **Workflow sequence**:
   ```
   implement changes → bump version → commit → build release → run benchmark → wait → analyze
   ```

---

## Known TODOs / Low-priority gaps

- **INT34-C: literal shift amount >= type width** — Current fix skips all non-negative
  integer literals to eliminate FPs from `x >> 8` etc. This means we miss the case where
  the literal is >= the promoted type width (e.g. `uint8_t x; x << 32;` — UB, 32 >= 32).
  Compilers warn on this with `-Wshift-count-overflow`, so low priority, but it could cause
  Juliet benchmark FNs on any test cases that use out-of-range literal shift amounts.
  Fix: skip only literals in `[0, 31]` for non-`long long` operands, `[0, 63]` for `long long`.
  Requires knowing the promoted operand type (non-trivial without type resolution).

---

## Benchmark Improvement Priorities (Juliet TP Rate)

The following CERT-C rules are **high priority** for Juliet benchmark improvement
because they supersede critical BISSELL code rules (BRULE-045, BRULE-047,
BRULE-051, BRULE-056). When selecting rules to improve, prefer these over others.

Within each group, focus on **increasing TP rate** (reducing false negatives) and
**reducing FP rate** (reducing false positives on `good*` functions).

### Tier 1 — Undefined Behavior (BRULE-047 → highest impact)
- **EXP34-C** — null pointer dereference (CWE-476; largest single CWE in Juliet)
  - Round 14 fix: `deref_after_check` pattern — `if (ptr == NULL) { *ptr; }` now caught
    via `end_byte` in `null_check_positions` + removed premature `null_checked_vars` early exit
    + `end_byte` in `nullable_reassignments` (prevents self-referential FP on `cur = cur->next`)
  - Round 15 fix: if/else branch merge — `collect_null_variables` now takes union of
    `potentially_null_vars` from both branches, fixing variant 12 (`globalReturnsTrueOrFalse()`)
    where if-branch sets ptr=NULL and else-branch sets ptr=non-null (+8 TPs)
  - Remaining gaps: variant 45 (static global null flow across functions — requires file-level
    pre-pass to track globals assigned NULL), multi-file splits (need interprocedural)
- **ARR30-C** / **ARR38-C** — out-of-bounds array access (CWE-125, CWE-787)
- **EXP33-C** — uninitialized memory reads (CWE-457)
- **INT30-C** / **INT32-C** — unsigned wraparound / signed integer overflow (CWE-190, CWE-191)

### Tier 2 — Memory Safety (BRULE-045)
- **MEM30-C** — use-after-free (CWE-416)
- **MEM31-C** — memory leak / failure to free (CWE-401)
- **MEM34-C** — double-free (CWE-415)
- **MEM35-C** — insufficient memory allocation (CWE-131)

### Tier 3 — Concurrency (BRULE-051)
- **CON30-C** through **CON43-C** — race conditions, deadlock, unsafe shared access
  (CWE-362, CWE-366, CWE-367)
- Focus: Juliet has limited concurrency test cases; prioritize FP reduction over TP gains here.

### Tier 4 — Sensitive Data (BRULE-056)
- **MSC41-C** — hard-coded credentials / sensitive literals (CWE-798, CWE-259)
- Focus: Juliet coverage is thin; improvements here are primarily real-world FP/FN quality.

---

## Project Structure

- `src/rules/cert_c/` - CERT C rule implementations
- `AGENTS/PROPOSALS/ACTIVE/` - Proposals to implement
- `AGENTS/PROPOSALS/STAGED/` - Completed proposals
- `scripts/work_active_helpers.sh` - Workflow automation

## Build & Test

```bash
cargo build
cargo test --package sqc --lib -- rules::cert_c::RULE_ID::tests
cargo fmt
```

## Git Commit Rules (CRITICAL)

**EXPLICITLY DENIED:**
- `git commit --no-verify` - NEVER use this flag. Pre-commit hooks MUST pass. Only humans can skip hooks.
- `Co-Authored-By: Claude` - NEVER add Claude as co-author. This is a corporate repository.
- Any hook-skipping flags (`--no-gpg-sign`, etc.)

**REQUIRED:**
- All pre-commit hooks must pass before commit succeeds
- If hooks fail, FIX the underlying issue (don't bypass)
- Standard commit message format without AI attribution

**Example Commit:**
```bash
git add files...
git commit -m "P2-RULE-ID: Implementation complete

- Brief description of changes
- Test results"
```
