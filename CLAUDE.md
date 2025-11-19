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
