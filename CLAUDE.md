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

## Known TODOs / Low-priority gaps

- **INT34-C: literal shift amount >= type width** — Current fix skips all non-negative
  integer literals to eliminate FPs from `x >> 8` etc. This means we miss the case where
  the literal is >= the promoted type width (e.g. `uint8_t x; x << 32;` — UB, 32 >= 32).
  Compilers warn on this with `-Wshift-count-overflow`, so low priority, but it could cause
  Juliet benchmark FNs on any test cases that use out-of-range literal shift amounts.
  Fix: skip only literals in `[0, 31]` for non-`long long` operands, `[0, 63]` for `long long`.
  Requires knowing the promoted operand type (non-trivial without type resolution).

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
