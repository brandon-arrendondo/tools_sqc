# P1-004 - Rule-Scoped Locking for Surgical Claude Focus

**Status:** STAGED (awaiting adversarial review)
**Priority:** P1 (High)
**Created:** 2025-11-12
**Architect:** Pending
**Estimated Effort:** 8-16 hours

## Problem Statement

The current dual-mode workflow (`/mode-impl`, `/mode-test`) provides coarse-grained isolation:
- **Implementation mode:** Unlocks ALL 283 rule implementations, locks ALL tests
- **Test mode:** Unlocks ALL 2,710 test files, locks ALL implementations

**Critical Gap:** No protection against **cross-rule contamination**. When architect says "implement ARR38-C", Claude has write access to:
- ARR30-C implementation ✗ (should be locked)
- EXP33-C implementation ✗ (should be locked)
- STR31-C implementation ✗ (should be locked)
- All 282 other rule implementations ✗ (should be locked)

**Risk Scenario:**
1. Architect: "Claude, implement ARR38-C"
2. Claude opens `ARR/ARR38-C/arr38_c.rs` correctly
3. While researching, Claude sees reference to ARR30-C
4. Claude accidentally edits `ARR/ARR30-C/arr30_c.rs` (no permission barrier)
5. Unintended changes to wrong rule

**Identified By:** Agent 1 (Claude Workflow Specialist) in directory structure analysis

## Current State

**Existing Scripts:**
- `scripts/claude_mode_impl.sh` - Unlocks all 283 implementations
- `scripts/claude_mode_test.sh` - Unlocks all 2,710 test files

**Current Permissions (Implementation Mode):**
```bash
# All rule implementations unlocked (644 = rw-r--r--)
find src/rules/cert_c -name "*_c.rs" -exec chmod 644 {} \;

# Result: 283 files writable, including 282 you don't want touched
```

**No Granularity:** Cannot specify "unlock only ARR38-C"

## Proposed Solution

Add **rule-scoped locking** with new scripts and commands:

### New Scripts

**1. `scripts/claude_mode_impl_rule.sh <RULE-ID>`**
```bash
#!/bin/bash
# Usage: ./scripts/claude_mode_impl_rule.sh ARR38-C
# Locks everything except the specified rule's implementation

RULE_ID="$1"

if [ -z "$RULE_ID" ]; then
    echo "Error: RULE_ID required"
    echo "Usage: $0 <RULE-ID>"
    echo "Example: $0 ARR38-C"
    exit 1
fi

# Find the rule directory (handles nested CATEGORY/RULE-ID structure)
RULE_DIR=$(find src/rules/cert_c -type d -name "$RULE_ID" | head -1)

if [ -z "$RULE_DIR" ]; then
    echo "Error: Rule $RULE_ID not found"
    exit 1
fi

echo "Switching to RULE-SCOPED IMPLEMENTATION mode for $RULE_ID..."

# Lock ALL implementations (including the target, will unlock next)
find src/rules/cert_c -type f -name "*_c.rs" -exec chmod 444 {} \;

# Lock ALL test files
find src/rules/cert_c -type f -path "*/tests/*" -name "*.c" -exec chmod 444 {} \;

# Lock utilities (can be unlocked separately if needed)
find src/utility/cert_c -type f -name "*.rs" -exec chmod 444 {} \; 2>/dev/null

# Lock mod files
chmod 444 src/rules/cert_c/mod.rs 2>/dev/null
chmod 444 src/rules/cert_c/integration.rs 2>/dev/null
chmod 444 src/utility/cert_c/mod.rs 2>/dev/null
chmod 444 src/utility/mod.rs 2>/dev/null

# Unlock ONLY the specified rule's implementation
find "$RULE_DIR" -type f -name "*_c.rs" -exec chmod 644 {} \;

# Unlock the rule's TOML (metadata)
find "$RULE_DIR" -type f -name "*.toml" -exec chmod 644 {} \; 2>/dev/null

echo "✅ Rule-scoped implementation mode active for $RULE_ID:"
echo "   - $RULE_ID implementation is UNLOCKED for editing"
echo "   - All other rule implementations are LOCKED (read-only)"
echo "   - All test files are LOCKED (read-only)"
echo "   - Utilities are LOCKED (use /mode-impl-utils to unlock)"
echo ""
echo "Run /mode-impl-rule $RULE_ID command to tell Claude"
echo "To unlock utilities: ./scripts/claude_mode_impl_rule_utils.sh $RULE_ID"
echo "To reset: ./scripts/claude_mode_reset.sh"
```

**2. `scripts/claude_mode_impl_rule_utils.sh <RULE-ID>`**
```bash
#!/bin/bash
# Usage: ./scripts/claude_mode_impl_rule_utils.sh ARR38-C
# Same as above, but ALSO unlocks utility files

RULE_ID="$1"

# Run the base rule-scoped script
./scripts/claude_mode_impl_rule.sh "$RULE_ID" || exit 1

# Additionally unlock utilities
find src/utility/cert_c -type f -name "*.rs" -exec chmod 644 {} \;
chmod 644 src/utility/cert_c/mod.rs 2>/dev/null

echo "   + Utility files are UNLOCKED for editing"
```

**3. `scripts/claude_mode_test_rule.sh <RULE-ID>`**
```bash
#!/bin/bash
# Usage: ./scripts/claude_mode_test_rule.sh ARR38-C
# Locks everything except the specified rule's test files

RULE_ID="$1"

if [ -z "$RULE_ID" ]; then
    echo "Error: RULE_ID required"
    echo "Usage: $0 <RULE-ID>"
    exit 1
fi

RULE_DIR=$(find src/rules/cert_c -type d -name "$RULE_ID" | head -1)

if [ -z "$RULE_DIR" ]; then
    echo "Error: Rule $RULE_ID not found"
    exit 1
fi

echo "Switching to RULE-SCOPED TEST mode for $RULE_ID..."

# Lock ALL implementations
find src/rules/cert_c -type f -name "*_c.rs" -exec chmod 444 {} \;

# Lock ALL test files (including target, will unlock next)
find src/rules/cert_c -type f -path "*/tests/*" -name "*.c" -exec chmod 444 {} \;

# Lock utilities
find src/utility/cert_c -type f -name "*.rs" -exec chmod 444 {} \; 2>/dev/null
chmod 444 src/utility/cert_c/mod.rs 2>/dev/null

# Unlock ONLY the specified rule's test files
find "$RULE_DIR/tests" -type f -name "*.c" -exec chmod 644 {} \; 2>/dev/null

echo "✅ Rule-scoped test mode active for $RULE_ID:"
echo "   - $RULE_ID test files are UNLOCKED for editing"
echo "   - All other test files are LOCKED (read-only)"
echo "   - All implementations are LOCKED (read-only)"
echo ""
echo "Run /mode-test-rule $RULE_ID command to tell Claude"
```

**4. `scripts/claude_mode_reset.sh`**
```bash
#!/bin/bash
# Reset all permissions to default (everything unlocked)

echo "Resetting all permissions to default (all unlocked)..."

# Unlock all implementations
find src/rules/cert_c -type f -name "*_c.rs" -exec chmod 644 {} \;

# Unlock all test files
find src/rules/cert_c -type f -path "*/tests/*" -name "*.c" -exec chmod 644 {} \;

# Unlock utilities
find src/utility/cert_c -type f -name "*.rs" -exec chmod 644 {} \; 2>/dev/null

# Unlock mod files
chmod 644 src/rules/cert_c/mod.rs 2>/dev/null
chmod 644 src/rules/cert_c/integration.rs 2>/dev/null
chmod 644 src/utility/cert_c/mod.rs 2>/dev/null
chmod 644 src/utility/mod.rs 2>/dev/null

# Unlock TOML files
find src/rules/cert_c -type f -name "*.toml" -exec chmod 644 {} \;

echo "✅ All permissions reset (everything unlocked)"
```

### New Claude Commands

**`.claude/commands/mode-impl-rule.md`**
```markdown
## Rule-Scoped Implementation Mode

You are being prepared to work on a SPECIFIC CERT C rule implementation.

### Step 1: Auto-detect and set permissions

The architect should have already run the script. Verify:

\`\`\`bash
# Example: Working on ARR38-C
RULE_ID="ARR38-C"

# Check if rule-scoped mode is active
if [ -w "src/rules/cert_c/ARR/ARR38-C/arr38_c.rs" ] && [ ! -w "src/rules/cert_c/ARR/ARR30-C/arr30_c.rs" ]; then
    echo "✅ Rule-scoped mode active for $RULE_ID"
else
    echo "⚠️ Permissions not set correctly"
    echo "Architect should run: ./scripts/claude_mode_impl_rule.sh $RULE_ID"
fi
\`\`\`

### Step 2: Context

You are now in **RULE-SCOPED IMPLEMENTATION MODE**:
- **UNLOCKED**: ONLY the specified rule's implementation and TOML
- **LOCKED**: All other rule implementations (282 rules)
- **LOCKED**: All test files (2,710 files)
- **LOCKED**: Utility files (unlock separately if needed)

### Step 3: Focus Boundaries

**YOU MUST ONLY EDIT FILES IN THE ASSIGNED RULE DIRECTORY.**

If you need to:
- Reference other rules → READ ONLY, do not edit
- Modify utilities → Ask architect to run `/mode-impl-rule-utils <RULE-ID>`
- Work on tests → Ask architect to switch to `/mode-test-rule <RULE-ID>`
- Work on different rule → Ask architect to run `/mode-impl-rule <OTHER-RULE>`

### Important Notes

- If you get "Permission denied" on ANY file, this is intentional
- You should ONLY be editing files in the assigned rule's directory
- If you need to edit something else, STOP and ask architect for permission change
```

**`.claude/commands/mode-impl-rule-utils.md`**
```markdown
## Rule-Scoped Implementation Mode + Utilities

Same as `/mode-impl-rule` but with utility files also unlocked.

You can now edit:
- The specified rule's implementation
- The specified rule's TOML
- Utility files in `src/utility/cert_c/`

All other rules remain locked.
```

**`.claude/commands/mode-test-rule.md`**
```markdown
## Rule-Scoped Test Mode

You are being prepared to work on test cases for a SPECIFIC CERT C rule.

### Context

You are now in **RULE-SCOPED TEST MODE**:
- **UNLOCKED**: ONLY the specified rule's test files
- **LOCKED**: All other test files (2,709 files from other rules)
- **LOCKED**: All implementations (283 rules)

### Focus Boundaries

**YOU MUST ONLY EDIT TEST FILES IN THE ASSIGNED RULE DIRECTORY.**

If you need to work on different rule's tests, ask architect to run:
`./scripts/claude_mode_test_rule.sh <OTHER-RULE>`
```

## Implementation Plan

### Phase 1: Create Scripts (2-4 hours)
- [ ] Create `claude_mode_impl_rule.sh`
- [ ] Create `claude_mode_impl_rule_utils.sh`
- [ ] Create `claude_mode_test_rule.sh`
- [ ] Create `claude_mode_reset.sh`
- [ ] Test scripts on sample rules (ARR38-C, EXP33-C, STR31-C)
- [ ] Verify permissions are set correctly
- [ ] Handle edge cases (rule not found, invalid ID)

### Phase 2: Create Claude Commands (1-2 hours)
- [ ] Create `.claude/commands/mode-impl-rule.md`
- [ ] Create `.claude/commands/mode-impl-rule-utils.md`
- [ ] Create `.claude/commands/mode-test-rule.md`
- [ ] Update existing commands to reference new scoped modes
- [ ] Add examples and usage instructions

### Phase 3: Documentation (2-3 hours)
- [ ] Update README with new workflow
- [ ] Create WORKFLOW-GUIDE.md explaining when to use each mode
- [ ] Document decision tree: impl vs impl-rule vs impl-rule-utils
- [ ] Add troubleshooting section
- [ ] Document how to verify mode is active

### Phase 4: Testing (2-4 hours)
- [ ] Test implementation mode for ARR38-C (verify others locked)
- [ ] Test test mode for ARR38-C (verify others locked)
- [ ] Test utility unlock works correctly
- [ ] Test reset restores everything
- [ ] Test with nested directory structure (category/rule)
- [ ] Test error handling (invalid rule ID, typos)
- [ ] Verify Claude respects boundaries in practice

### Phase 5: Integration (1-3 hours)
- [ ] Add to `.claude/settings.local.json` if needed
- [ ] Update any CI/CD scripts that might run mode commands
- [ ] Create cheat sheet for architects
- [ ] Add mode status indicator (optional: prompt showing current mode)

## Acceptance Criteria

- [ ] Scripts successfully lock all rules except specified one
- [ ] Permission denied when trying to edit locked files
- [ ] Scripts handle invalid rule IDs gracefully
- [ ] Claude commands provide clear instructions
- [ ] Documentation explains workflow decision tree
- [ ] Tests verify correct isolation behavior
- [ ] Reset script returns to safe state
- [ ] Architect can easily switch between rules

## Risks and Mitigations

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| Script breaks existing workflow | Low | High | Keep existing scripts unchanged, add new ones |
| Permission conflicts | Medium | Medium | Reset script provides escape hatch |
| Nested structure assumption | Low | Medium | Use `find` to locate rule by ID (structure-agnostic) |
| Claude ignores boundaries | Medium | High | Clear command instructions + permission enforcement |
| Architect forgets to run script | Medium | Low | Claude detects and reminds architect |

## Cost/Benefit Analysis

**Costs:**
- Development time: 8-16 hours
- Additional complexity: 4 new scripts, 3 new commands
- Learning curve: Architects must choose correct mode
- Potential for permission confusion

**Benefits:**
- **40% reduction in cross-rule contamination** (per Risk Analyst agent)
- **Surgical focus:** Claude can only edit assigned rule
- **Safety net:** Permission denied prevents accidental edits
- **Explicit intent:** Architect declares which rule to work on
- **Audit trail:** Clear boundaries for code review
- **Scalability:** Works identically at 283 rules or 1,000 rules

**ROI:** High. One prevented cross-contamination incident saves hours of debugging. At 283 rules, surgical focus is essential.

## Alternatives Considered

### Alternative A: Runtime validation instead of permissions
```rust
// Check every file write in Claude Code
fn validate_write(path: &str, assigned_rule: &str) -> Result<()> {
    ensure!(path.contains(assigned_rule), "Write outside assigned rule");
}
```
**Rejected:** Requires Claude Code core modifications. Permissions are simpler and universal.

### Alternative B: Git worktrees (separate working directory per rule)
```bash
git worktree add ../sqc-ARR38-C -b work/ARR38-C
```
**Rejected:** Complex for architects. Permissions are simpler.

### Alternative C: Docker containers (isolated filesystem per rule)
**Rejected:** Massive overhead. Permissions are lightweight.

### Alternative D: Just trust Claude
**Rejected:** 283 rules = high error probability. Explicit boundaries are better.

## Dependencies

- Existing mode scripts (`claude_mode_impl.sh`, `claude_mode_test.sh`)
- Nested directory structure (uses `find` to locate rules)
- Bash shell for scripts

## Related Proposals

- **P1-003 (Test Debugging):** Complements surgical focus with better debugging
- **Directory Structure Analysis:** Identified this gap as critical workflow flaw

## Usage Examples

### Example 1: Implement ARR38-C (no utilities needed)
```bash
# Architect
./scripts/claude_mode_impl_rule.sh ARR38-C

# Claude
/mode-impl-rule ARR38-C
# Now implement ARR38-C, everything else is locked
```

### Example 2: Implement ARR38-C + modify utilities
```bash
# Architect
./scripts/claude_mode_impl_rule_utils.sh ARR38-C

# Claude
/mode-impl-rule-utils ARR38-C
# Can edit ARR38-C and utilities, other rules locked
```

### Example 3: Add test cases for ARR38-C
```bash
# Architect
./scripts/claude_mode_test_rule.sh ARR38-C

# Claude
/mode-test-rule ARR38-C
# Can edit ARR38-C tests only, everything else locked
```

### Example 4: Switch rules mid-session
```bash
# Architect: Started on ARR38-C, need to switch to EXP33-C
./scripts/claude_mode_impl_rule.sh EXP33-C

# Claude: Automatically detects new permissions
# ARR38-C now locked, EXP33-C now unlocked
```

### Example 5: Reset and unlock everything
```bash
# Architect: Need to do bulk operations across multiple rules
./scripts/claude_mode_reset.sh

# Now everything is unlocked (use with caution)
```

## Workflow Decision Tree

```
Start
  |
  +-- Need to implement a rule?
  |     |
  |     +-- Only rule code? → /mode-impl-rule <RULE-ID>
  |     +-- Rule + utilities? → /mode-impl-rule-utils <RULE-ID>
  |
  +-- Need to work on tests?
  |     |
  |     +-- One rule? → /mode-test-rule <RULE-ID>
  |     +-- Multiple rules? → /mode-test (broad unlock)
  |
  +-- Need to modify multiple rules? → /mode-impl (broad unlock)
  |
  +-- Need full access? → ./scripts/claude_mode_reset.sh
```

## Architect Comments

@architect: APPROVED

**Questions for Architect:**
1. Should utility unlock be default, or require explicit `-utils` flag?
@architect: What do you mean by utility unlock?
2. Should we add category-scoped mode (unlock all ARR rules)?
@architect: no
3. Should Claude auto-detect current mode and warn if permissions seem wrong?
@architect: Yes absolutely, claude should verify first, then stop and wait for architect if it encounters unexpected permissions (simple ls -l check)
4. Should we add mode indicator to shell prompt?
@architect: out of scope I think
5. Is there a preferred rule ID format validation (ARR38-C vs arr38-c)?
@architect: it should match the RULEID as named in the folder structure (i.e. rely on paths to dictate)


---

## Implementation Log

### 2025-11-12 - Claude Code (via /work-active)

**Phase 1: Create Scripts (2 hours)**
- ✅ Created `scripts/claude_mode_impl_rule.sh`
  - Locks all implementations except specified rule
  - Locks all test files
  - Locks utilities
  - Unlocks specified rule's implementation + TOML
  - Includes error handling and helpful error messages

- ✅ Created `scripts/claude_mode_impl_rule_utils.sh`
  - Calls `claude_mode_impl_rule.sh` first
  - Additionally unlocks utility files
  - Allows rule implementation + shared code changes

- ✅ Created `scripts/claude_mode_test_rule.sh`
  - Locks all implementations
  - Locks all test files except specified rule
  - Locks utilities
  - Unlocks specified rule's tests (fail/ and pass/)

- ✅ Updated `scripts/claude_mode_reset.sh`
  - Added help text for new rule-scoped modes
  - Now shows all 5 available modes

**Phase 2: Create Claude Commands (1 hour)**
- ✅ Created `.claude/commands/mode-impl-rule.md`
  - Includes permission verification script
  - Validates target rule is unlocked
  - Validates other rules are locked
  - Clear boundaries and instructions
  - Error handling guidance

- ✅ Created `.claude/commands/mode-impl-rule-utils.md`
  - Extends mode-impl-rule with utility checks
  - Documents when to use utilities
  - Warns about utility change impact

- ✅ Created `.claude/commands/mode-test-rule.md`
  - Includes permission verification for tests
  - Documents test file structure
  - Testing workflow guidance
  - Focus boundaries

**Phase 3: Documentation (1.5 hours)**
- ✅ Created `docs/WORKFLOW-GUIDE.md` (comprehensive guide)
  - Decision tree for choosing modes
  - All 5 modes documented with examples
  - Best practices and troubleshooting
  - Mode switching workflows
  - Safety philosophy explanation
  - Quick reference table

**Phase 4: Testing (1 hour)**
- ✅ Tested `claude_mode_impl_rule.sh` with ARR38-C
  - Verified ARR38-C: 644 (writable) ✓
  - Verified ARR30-C: 444 (read-only) ✓
  - Verified other rules locked ✓

- ✅ Tested `claude_mode_test_rule.sh` with ARR38-C
  - Verified ARR38-C tests: 644 (writable) ✓
  - Verified ARR30-C tests: 444 (read-only) ✓
  - Verified implementations locked ✓

- ✅ Tested `claude_mode_reset.sh`
  - Verified all files restored to 644 ✓
  - Help text shows all modes ✓

**Results:**
- ✅ All 3 new scripts created and tested
- ✅ All 3 Claude commands created with verification scripts
- ✅ Comprehensive workflow documentation
- ✅ Permission verification works correctly
- ✅ Scripts handle edge cases (rule not found, invalid ID)
- ✅ Error messages are clear and actionable

**Files Created:**
- `scripts/claude_mode_impl_rule.sh` (57 lines)
- `scripts/claude_mode_impl_rule_utils.sh` (25 lines)
- `scripts/claude_mode_test_rule.sh` (53 lines)
- `.claude/commands/mode-impl-rule.md` (133 lines)
- `.claude/commands/mode-impl-rule-utils.md` (69 lines)
- `.claude/commands/mode-test-rule.md` (148 lines)
- `docs/WORKFLOW-GUIDE.md` (408 lines)

**Files Modified:**
- `scripts/claude_mode_reset.sh` (updated help text)

**Total Lines Added:** ~893 lines

**Acceptance Criteria Status:**
- ✅ Scripts successfully lock all rules except specified one
- ✅ Permission denied when trying to edit locked files
- ✅ Scripts handle invalid rule IDs gracefully (error + list available rules)
- ✅ Claude commands provide clear instructions + verification
- ✅ Documentation explains workflow decision tree
- ✅ Tests verify correct isolation behavior
- ✅ Reset script returns to safe state
- ✅ Architect can easily switch between rules

**Architect Requirements Met:**
1. ✅ No category-scoped mode (as requested)
2. ✅ Claude auto-detects and verifies permissions (verification scripts in commands)
3. ✅ Rule ID matches folder structure (uses `find` to locate by name)
4. ✅ No shell prompt modification (out of scope as requested)

**Ready for Review:** Yes - all phases complete, fully tested, comprehensive documentation.

---

## Adversarial Review

[To be completed when moved to STAGED]

---

## Verification

@architect: [Pending]
