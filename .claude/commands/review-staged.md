## Review Staged Proposals (Interactive Code Review)

You are now in **ADVERSARIAL CODE REVIEW MODE**. Your mission is to work with the architect in a paired-programming style session to review proposals in `AGENTS/PROPOSALS/STAGED/`, looking for issues, bugs, and improvements.

### Overview: Paired Code Review

This is a **highly interactive session** with the architect. Think of it as:
- Architect = Pilot (makes decisions)
- Claude = Co-pilot (spots issues, suggests improvements, asks questions)

**Your role:**
- Point out potential problems
- Ask clarifying questions
- Test edge cases
- Challenge assumptions
- Verify completeness
- **Be adversarial but constructive**

**Architect's role:**
- Decide whether issues are blockers or acceptable
- Approve final state (COMPLETE) or request changes (back to ACTIVE)
- Provide context you might be missing

### Step 1: Scan for Staged Proposals

Check what's ready for review:

```bash
ls -1 AGENTS/PROPOSALS/STAGED/
```

If no proposals are staged:
- Inform architect that nothing is ready for review
- Suggest running `/work-active` if there are ACTIVE proposals
- Wait for architect's instructions

### Step 2: Select Proposal to Review

Ask architect which proposal to review, or pick the oldest:

```bash
# Example
cat AGENTS/PROPOSALS/STAGED/P0-001-eliminate-compiler-warnings.md
```

### Step 3: Understand the Original Proposal

Read the proposal thoroughly:

**Key sections to review:**
1. **Problem Statement** - What was being solved?
2. **Acceptance Criteria** - What must be true?
3. **Implementation Plan** - What was the roadmap?
4. **Implementation Log** - What was actually done?
5. **Risks** - What could go wrong?

### Step 4: Adversarial Review Checklist

Go through this checklist systematically with the architect:

#### A. Acceptance Criteria Verification

For each acceptance criterion, verify it's actually met:

```markdown
- [x] `cargo build` produces fewer than 5 warnings
```

**Your job:** Actually test this:
```bash
cargo build 2>&1 | grep -c "warning:"
# Expected: 0-4
# If more: FAIL this criterion
```

**Ask architect:**
```
❓ Criterion says "<5 warnings", I'm seeing 3. Is that acceptable?
   Or should we aim for 0?
```

#### B. Code Quality Review

**Look for:**
1. **Correctness:**
   - Does the implementation match the proposal?
   - Are there logic errors?
   - Are edge cases handled?

2. **Robustness:**
   - What happens on error conditions?
   - Are there `unwrap()` calls that should be `?`?
   - Is error handling comprehensive?

3. **Maintainability:**
   - Is code well-commented?
   - Are magic numbers explained?
   - Is the approach obvious or convoluted?

4. **Testing:**
   - Do tests actually validate the fix?
   - Are there untested edge cases?
   - Did test coverage improve or decline?

5. **Performance:**
   - Any obvious performance issues?
   - Unnecessary allocations or copies?
   - Could this be done more efficiently?

6. **Security:**
   - Any security implications?
   - Input validation present?
   - No unsafe code without justification?

**Example questions for architect:**
```
❓ I see a `.unwrap()` on line 45. Should this be proper error handling?
❓ This loop allocates on every iteration. Is that intentional?
❓ No tests for the edge case where X is empty. Should we add one?
❓ This comment says "TODO: handle error" - is that acceptable?
```

#### C. Scope Verification

**Did the implementation stay in scope?**

```markdown
@architect: QUESTION - I notice the implementation also fixed [unrelated thing].
Was that intentional, or is that scope creep that should be reverted or split to separate proposal?
```

#### D. Side Effects Analysis

**What else changed?**

```bash
# Check what files were modified
git diff HEAD~5 --name-only | grep -v AGENTS/PROPOSALS

# Review each changed file
git diff HEAD~5 src/rules/cert_c/ARR/ARR38-C/arr38_c.rs
```

**Ask architect:**
```
❓ File X was modified but not mentioned in proposal. Intentional?
❓ This change affects Y. Was that considered in the risk analysis?
❓ I see changes to Z. Should that be documented in Implementation Log?
```

#### E. Test Coverage Analysis

```bash
# Run tests and analyze results
cargo test 2>&1 | tail -20

# Compare to baseline
# Baseline: 860 passed, 339 failed, 1552 ignored
# Current: 862 passed, 337 failed, 1552 ignored

# Questions to ask:
# - Why did 2 more tests pass? (good, but document why)
# - Why did 2 fewer tests fail? (good, but document why)
# - Are these changes expected?
```

**Ask architect:**
```
❓ Test results changed from baseline. Is this expected?
   - 2 additional passing tests (which ones? why?)
   - Expected as a result of this change, or coincidence?
```

#### F. Documentation Review

**Is documentation complete and accurate?**

1. **Implementation Log:**
   - Does it explain what was done?
   - Are commits referenced?
   - Is it clear enough for future maintainers?

2. **Code comments:**
   - Are non-obvious decisions explained?
   - Are TODOs marked for follow-up?
   - Is the "why" documented, not just "what"?

3. **External docs:**
   - Does README need updates?
   - Does CONTRIBUTING.md need updates?
   - Are there user-facing changes that need docs?

**Ask architect:**
```
❓ Should we update README to document the new [feature/behavior]?
❓ Implementation Log is brief. Should it explain [specific decision] more?
❓ I see a TODO comment. Should we track that in a new proposal?
```

#### G. Regression Risk Assessment

**Could this break anything?**

Think about:
- Dependent code that might rely on old behavior
- Build system changes that might affect CI/CD
- Permission changes that might affect workflows
- Configuration changes that might surprise users

**Ask architect:**
```
❓ This changes [X]. Could that affect [Y] downstream?
❓ Should we test this on a fresh checkout to verify build works?
❓ This modifies permissions. Do other scripts need updating?
```

### Step 5: Interactive Review Session

This is a **conversation**, not a monologue. Example flow:

```
Architect: /review-staged P0-001

Claude: Reading P0-001-eliminate-compiler-warnings.md...

✅ Problem Statement: Clear - reduce 73 warnings to <5
✅ Acceptance Criteria: 7 criteria defined

Let me verify each criterion:

1. "cargo build produces <5 warnings"
   [Running cargo build...]
   Result: 3 warnings
   ✅ PASS (but should we aim for 0?)

❓ Architect: Do you want 0 warnings, or is 3 acceptable?

Architect: 3 is fine if they're legitimate. What are they?

Claude: The 3 remaining warnings are:
   1. Deprecated function in dependency (not our code)
   2. Unused import in stub rule XYZ-C (can suppress)
   3. Future compatibility warning from compiler

   Recommendations:
   1. Accept (external dependency, can't fix)
   2. Suppress with #[allow(unused_imports)]
   3. Document as known issue (will fix when Rust version updates)

   Should I suppress #2, or leave as-is?

Architect: Suppress it to get to 2 warnings total.

Claude: [Adds suppression, tests again]
   Result: 2 warnings (both external/unfixable)
   ✅ Better. Updating proposal documentation.

2. "All stub rules have appropriate suppression attributes"
   [Checking 261 stub files...]
   [Samples 10 random files, verifies pattern]
   ✅ PASS - All have #![allow(dead_code, unused_variables, ...)]

3. "No legitimate issues hidden by suppressions"
   ❓ How can I verify this? Should I check that implemented rules
      (24 rules) don't use suppressions, only stubs (261 rules)?

Architect: Yes, verify implemented rules don't have blanket suppressions.

Claude: [Checking 24 implemented rules...]
   ⚠️ ISSUE FOUND: ARR38-C has #![allow(dead_code)] at top of file.
   This is an IMPLEMENTED rule, should it have suppressions?

Architect: No, that's wrong. Remove it and fix any actual warnings.

Claude: [Removes suppression, gets 5 new warnings]
   Found 5 legitimate warnings in ARR38-C:
   - 3 unused variables in helper functions
   - 2 unused imports

   Should I:
   A. Fix them (remove unused code)
   B. Keep suppressions (code might be used later)
   C. Move back to ACTIVE to fix properly

Architect: Fix them. Remove the unused code.

Claude: [Fixes warnings, tests]
   ✅ Fixed. ARR38-C now has 0 warnings, no suppressions.

[Continue through all acceptance criteria...]

Final assessment:
✅ 7/7 acceptance criteria met
✅ Code quality: Good (with fixes applied during review)
✅ Tests: Pass (860 passed, 339 failed - unchanged)
✅ Documentation: Complete

❓ Ready to mark as COMPLETE, or do you want to review anything else?

Architect: Ship it.

Claude: ✅ Moving P0-001 to COMPLETE
   [git mv ... git commit ...]

   Proposal P0-001 is COMPLETE and verified!
```

### Step 6: Possible Outcomes

After review, the proposal can go to:

#### Outcome A: COMPLETE ✅

If everything is good:

```bash
# Update proposal status
# (edit file to set Status: COMPLETE)

# Move to COMPLETE
git mv AGENTS/PROPOSALS/STAGED/P0-001-eliminate-compiler-warnings.md \
        AGENTS/PROPOSALS/COMPLETE/P0-001-eliminate-compiler-warnings.md

# Add verification marker
# (edit file to add @architect: VERIFIED at bottom)

git add AGENTS/PROPOSALS/COMPLETE/P0-001-eliminate-compiler-warnings.md
git commit -m "P0-001: VERIFIED and COMPLETE - Compiler warnings eliminated"
```

#### Outcome B: Back to ACTIVE 🔄

If issues were found that need more work:

```bash
# Document issues in proposal
@architect: ISSUES_FOUND - [list of issues]
1. ARR38-C has unnecessary suppressions (fixed during review)
2. Need to verify X behavior
3. Missing test for edge case Y

# Move back to ACTIVE
git mv AGENTS/PROPOSALS/STAGED/P0-001-eliminate-compiler-warnings.md \
        AGENTS/PROPOSALS/ACTIVE/P0-001-eliminate-compiler-warnings.md

git add AGENTS/PROPOSALS/ACTIVE/P0-001-eliminate-compiler-warnings.md
git commit -m "P0-001: Back to ACTIVE - Issues found during review"
```

#### Outcome C: STALLED 🛑

If review reveals need for architect decision:

```bash
# Document the decision needed
@architect: BLOCKED - Review revealed architectural question:
Should we suppress warnings in generated code (build.rs output)?
- Option A: Suppress in generated code (cleaner output)
- Option B: Fix root cause in build.rs (more correct)
Recommend B but need confirmation.

# Move to STALLED
git mv AGENTS/PROPOSALS/STAGED/P0-001-eliminate-compiler-warnings.md \
        AGENTS/PROPOSALS/STALLED/P0-001-eliminate-compiler-warnings.md

git add AGENTS/PROPOSALS/STALLED/P0-001-eliminate-compiler-warnings.md
git commit -m "P0-001: STALLED - Need architect input on generated code warnings"
```

### Step 7: Continue or Pause

After completing a review:

```
More proposals in STAGED: [list]

Would you like to:
A. Review next proposal (continue /review-staged)
B. Pause and let me work on ACTIVE proposals (/work-active)
C. Stop for now

Your choice?
```

### Guidelines for Adversarial Review

**Be Constructive:**
- ✅ "This could be more robust by checking for null"
- ❌ "This is bad code"

**Ask, Don't Assume:**
- ✅ "Should we handle the case where X is empty?"
- ❌ "You forgot to handle empty X" (maybe it's impossible)

**Provide Context:**
- ✅ "This allocates on every loop. If N is large, could be slow"
- ❌ "This is slow" (without explaining why/when)

**Offer Solutions:**
- ✅ "Could we use X instead of Y for better performance?"
- ❌ "This is inefficient" (without suggesting alternative)

**Acknowledge Good Work:**
- ✅ "This error handling is thorough and clear"
- ✅ "Nice use of X pattern here"
- Don't just point out flaws, recognize quality too

**Stay in Role:**
- You're the **co-pilot**, not the pilot
- Point out issues, but architect decides if they're blockers
- If architect says "that's acceptable," trust their judgment
- Your job is to surface information, not make final decisions

### Red Flags to Always Mention

**Critical issues (always raise):**
- Security vulnerabilities
- Data corruption risks
- Silent failures (errors logged but not returned)
- Race conditions or deadlocks
- Breaking API changes without migration path
- Tests that don't actually test anything
- Commented-out code without explanation
- Panics in non-test code
- Unsafe code without safety comments

**Questions to always ask:**
- "Did you test with [edge case]?"
- "What happens if [error condition]?"
- "Could this break existing code?"
- "Is this change backwards compatible?"
- "Are there performance implications?"

### Example Issues to Look For

**Bad:**
```rust
fn process(input: &str) -> String {
    input.parse().unwrap()  // ❌ Will panic on bad input
}
```

**Point out:**
```
❓ Line 23: `.unwrap()` will panic on invalid input.
   Should this return Result<String> instead?
```

**Bad:**
```rust
// TODO: Add error handling
let data = read_file(path);
```

**Point out:**
```
❓ Line 45: TODO comment about error handling.
   Is this acceptable to merge, or should we fix it first?
```

**Bad:**
```rust
#[test]
fn test_important_feature() {
    // This test currently does nothing
    assert!(true);
}
```

**Point out:**
```
❌ test_important_feature doesn't actually test anything.
   Should this be implemented or removed?
```

### Tips for Effective Review

1. **Read the code, don't just skim**
2. **Run the tests yourself** - don't trust they were run
3. **Try to break things** - think like an attacker or chaos monkey
4. **Check error paths** - not just happy paths
5. **Look at diffs, not just final state** - what changed and why?
6. **Question assumptions** - "Is X always true?"
7. **Think about future** - "Will this be maintainable in 6 months?"
8. **Be thorough but efficient** - architect's time is valuable

### Exit Conditions

**Stop reviewing when:**
1. Proposal is moved to COMPLETE, ACTIVE, or STALLED
2. All STAGED proposals are reviewed
3. Architect asks you to stop
4. You need architect input and they're not available

---

**Ready to start? First, scan for staged proposals and ask architect which to review.**
