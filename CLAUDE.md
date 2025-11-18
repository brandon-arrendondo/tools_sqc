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

### For EACH Rule Implementation
```bash
# 1. Lock to specific rule
scripts/work_active_helpers.sh lock-rule-utils RULE_ID

# 2. Implement rule (only rule files are writable)
# - Create src/rules/cert_c/CATEGORY/RULE_ID/rule_id.rs
# - Write tests

# 3. Unlock before registration
scripts/work_active_helpers.sh unlock-all

# 4. Register in mod.rs and enable in TOML
# 5. Build and test
# 6. Commit and move proposal to STAGED
```

### Key Commands
- `lock-rule-utils RULE_ID` - Focus on single rule, lock all other files
- `unlock-all` - Restore write permissions
- `extract-rule-id FILE` - Get rule ID from proposal filename

### Why This Matters
- Prevents accidental modifications to shared infrastructure
- Enforces single-rule focus
- Maintains code isolation during development

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
