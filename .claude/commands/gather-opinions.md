## Gather Opinions - Distributed Autonomous Review

**Purpose:** Rapid, persona-driven analysis of all STAGED proposals to gather multiple perspectives before final review.

**Mode:** Autonomous with initial persona selection

**Prerequisites:** Proposals in `AGENTS/PROPOSALS/STAGED/` awaiting review

---

### Step 1: Select Review Persona (INTERACTIVE)

Before reviewing proposals, I need to understand what perspective you want me to adopt.

**Common Review Personas:**

1. **Security Auditor**
   - Focus: Vulnerabilities, unsafe code, input validation, panic vectors
   - Looks for: unwrap(), unsafe{}, unvalidated input, SQL injection, XSS, buffer overflows

2. **Performance Engineer**
   - Focus: Algorithmic complexity, resource usage, efficiency
   - Looks for: O(n²) algorithms, unnecessary allocations, blocking operations, memory leaks

3. **Maintainability Advocate**
   - Focus: Code clarity, documentation, technical debt, future-proofing
   - Looks for: Complex logic, missing comments, magic numbers, tight coupling

4. **Test Quality Reviewer**
   - Focus: Test coverage, edge cases, test design, failure scenarios
   - Looks for: Missing tests, weak assertions, untested error paths, brittle tests

5. **API Designer**
   - Focus: Interfaces, ergonomics, breaking changes, backwards compatibility
   - Looks for: Poor naming, inconsistent patterns, breaking API changes, missing docs

6. **Memory Safety Expert**
   - Focus: Memory leaks, use-after-free, buffer overflows, lifetime issues
   - Looks for: Manual memory management, pointer arithmetic, unsafe casts

7. **Generalist**
   - Focus: Balanced review across all dimensions
   - Looks for: General correctness, completeness, quality

8. **Custom** - Describe your own perspective

**Which persona should I adopt?**

**ARCHITECT: REGARDLESS OF PERSONA, ALWAYS PERFORM AN EXAMINATION DURING THIS ANALYSIS IN AN ADVERSARIAL WAY, AND ALWAYS LOOK FOR DRY (Dont repeat yourself) and KISS (Keep it simple stupid) violations. Specific focus area is shared utility usage - all rules need to ensure that their error message is clearly and cleanly populated out**

---

### Step 2: Identify Reviewer

```bash
REVIEWER=$(git config user.name | tr ' ' '-' | tr '[:upper:]' '[:lower:]')
echo "Reviewer: $REVIEWER"
echo "Persona: {SELECTED_PERSONA}"
```

---

### Step 3: Scan STAGED Proposals

```bash
# Count proposals awaiting review
PROPOSAL_COUNT=$(ls -1 AGENTS/PROPOSALS/STAGED/*.md 2>/dev/null | wc -l)
echo "Found $PROPOSAL_COUNT proposals in STAGED/"

# List all proposals
ls -1 AGENTS/PROPOSALS/STAGED/*.md
```

**Starting autonomous review as {PERSONA}...**

---

### Step 4: Process Each Proposal (AUTONOMOUS)

For **EACH** proposal in STAGED, I will:

#### A. Read Complete Proposal
```bash
# Read entire proposal - ALL sections
cat AGENTS/PROPOSALS/STAGED/{PROPOSAL_FILE}.md
```

**Focus areas based on persona:**
- Read all sections: Task, Implementation Constraints, Implementation Log, Acceptance Criteria
- Review Implementation Log for what was actually done
- Check Acceptance Criteria for completeness
- Note any red flags specific to my persona

#### B. Quick Code Inspection (if commit referenced)
```bash
# Extract commit hash from Implementation Log (if present)
COMMIT=$(grep -oP 'commit[: ]+\K[a-f0-9]{7,40}' AGENTS/PROPOSALS/STAGED/{PROPOSAL_FILE}.md | head -1)

# View changes if commit found
if [ -n "$COMMIT" ]; then
  git show "$COMMIT" --stat
  git show "$COMMIT" -- src/rules/cert_c/ | head -100
fi
```

#### C. Form Opinion (Persona-Specific Analysis)

**Opinion Categories:**
- ✅ **LOOKS_GOOD** - Implementation appears complete, no issues from my persona's perspective
- ⚠️  **NEEDS_REVIEW** - Potential issues found that need addressing
- 🛑 **BLOCKED** - Critical issues from my persona's perspective, cannot proceed

**Persona-Specific Checklist:**

**If Security Auditor:**
- [ ] No unwrap() on user/external input
- [ ] No unsafe{} blocks without justification
- [ ] Input validation present
- [ ] No SQL injection vectors
- [ ] No buffer overflow risks
- [ ] Error handling doesn't leak sensitive info

**If Performance Engineer:**
- [ ] Algorithmic complexity reasonable (no O(n²) where O(n) possible)
- [ ] No unnecessary allocations in hot paths
- [ ] No blocking operations without timeout
- [ ] Resource cleanup (no leaks)
- [ ] Efficient data structures chosen

**If Maintainability Advocate:**
- [ ] Code is readable and well-structured
- [ ] Complex logic has explanatory comments
- [ ] No magic numbers (use constants)
- [ ] Functions are appropriately sized
- [ ] Consistent with codebase patterns
- [ ] Technical debt noted if introduced

**If Test Quality Reviewer:**
- [ ] Test coverage adequate for rule complexity
- [ ] Edge cases tested
- [ ] Error paths tested
- [ ] Tests use appropriate assertions
- [ ] Test names are descriptive
- [ ] No brittle tests (hardcoded values, timing-dependent)

**If API Designer:**
- [ ] API names are clear and consistent
- [ ] No breaking changes to public APIs
- [ ] Documentation updated if API changed
- [ ] Backwards compatibility maintained
- [ ] Error types appropriate

**If Memory Safety Expert:**
- [ ] No manual memory management issues
- [ ] No unsafe pointer arithmetic
- [ ] Proper lifetime management
- [ ] No use-after-free risks
- [ ] Buffer boundaries checked

**If Generalist:**
- [ ] Implementation matches task description
- [ ] All acceptance criteria met
- [ ] No obvious bugs
- [ ] Tests present and passing
- [ ] Code quality acceptable

#### D. Record Opinion
```bash
# Add opinion to proposal frontmatter
scripts/review_helpers.sh add-opinion \
  "AGENTS/PROPOSALS/STAGED/{PROPOSAL_FILE}.md" \
  "{SELECTED_PERSONA}" \
  "{OPINION}" \
  "{COMMENT}"

**ARCHITECT: YOUR COMMENT SHOULD ALWAYS BE CLEAR AND PROVIDE ENOUGH CONTEXT FOR A DEEPER DIVE INTO SPECIFICS** 

# Example:
# scripts/review_helpers.sh add-opinion \
#   "AGENTS/PROPOSALS/STAGED/P1-FIO37-C-implementation.md" \
#   "Security Auditor" \
#   "NEEDS_REVIEW" \
#   "Line 142: unwrap() on user input. Use ? operator or proper error handling."
```

**Comment Guidelines:**
- Be specific: cite line numbers, function names, file paths
- Be constructive: suggest fixes, not just problems
- Be brief: 1-2 sentences max
- Be persona-focused: comment on issues relevant to your perspective

#### E. Track Progress
```bash
# Simple progress counter
echo "✓ [{CURRENT}/{TOTAL}] {PROPOSAL_ID} - {OPINION}"
```

---

### Step 5: Generate Summary Report

After processing all proposals:

```
=== Gather Opinions Summary ===
Reviewer: {REVIEWER_NAME}
Persona: {SELECTED_PERSONA}
Date: {YYYY-MM-DD}
Session Duration: {HOURS} hours

Processed: {TOTAL} proposals
Average time: ~3 minutes per proposal

Opinions:
  ✅ LOOKS_GOOD: {COUNT} proposals
  ⚠️  NEEDS_REVIEW: {COUNT} proposals
  🛑 BLOCKED: {COUNT} proposals

Top Concerns (from {PERSONA} perspective):
  - {Issue 1}: {COUNT} proposals
  - {Issue 2}: {COUNT} proposals
  - {Issue 3}: {COUNT} proposals

Examples:
  LOOKS_GOOD: P2-API04-C (clean implementation, no issues)
  NEEDS_REVIEW: P1-FIO37-C (unwrap() usage)
  BLOCKED: P1-ARR30-C (unsafe pointer arithmetic without bounds check)

Next Steps:
  - All proposals now have your opinion recorded
  - Waiting for other reviewers to complete their opinions
  - Architect will run /review-staged to synthesize opinions
```

---

### Step 6: Check Coverage (Optional)

```bash
# See how many reviewers have reviewed each proposal
scripts/review_helpers.sh analyze-coverage
```

**Coverage Report:**
```
=== Review Coverage ===
Total proposals in STAGED: 93

Coverage:
  3+ reviewers: 45 proposals (good coverage)
  2 reviewers: 30 proposals (minimal coverage)
  1 reviewer: 15 proposals (needs more opinions)
  0 reviewers: 3 proposals (NOT REVIEWED)

Recommendation:
  - Proposals with 2+ reviewers ready for /review-staged
  - Proposals with 0-1 reviewers need more opinions
```

---

### Important Guidelines

**DO:**
- Read the COMPLETE proposal (all sections)
- Apply your persona's lens consistently
- Be specific in comments (line numbers, file paths)
- Record opinion even if "LOOKS_GOOD" (silence = no opinion)
- Process all proposals (don't cherry-pick)
- Stay objective and constructive

**DON'T:**
- Skip proposals (process all in STAGED)
- Modify code (this is read-only review)
- Make final decisions (that's Phase 2 /review-staged)
- Spend >5 minutes per proposal (this is rapid screening)
- Second-guess other reviewers' opinions (add yours independently)

**Time Management:**
- Budget: 3-5 hours for ~93 proposals
- Target: 2-4 minutes per proposal
- If going over: focus on Implementation Log + Acceptance Criteria only
- Deep analysis happens in Phase 2 (by architect)

---

### Exit Conditions

**Session Complete When:**
- All proposals in STAGED/ have your opinion recorded
- Summary report generated
- No modifications made to code (read-only)

**Next Steps:**
- Wait for other team members to complete their opinions
- Architect runs `/review-staged` to synthesize all opinions and make final decisions

---

### Example Session Flow

```
User: /gather-opinions

Claude: Which review persona should I adopt?
[Shows 8 options]

User: Security Auditor

Claude: Starting review as Security Auditor...
Reviewer: tristan-vanfossen
Found 93 proposals in STAGED/

Reading P1-API00-C-implementation.md... ✓ [1/93] LOOKS_GOOD
Reading P1-API01-C-implementation.md... ✓ [2/93] NEEDS_REVIEW
Reading P1-API02-C-implementation.md... ✓ [3/93] LOOKS_GOOD
...
Reading P2-WIN30-C-implementation.md... ✓ [93/93] BLOCKED

=== Summary ===
Processed: 93 proposals in 4.2 hours
  ✅ LOOKS_GOOD: 65
  ⚠️  NEEDS_REVIEW: 23
  🛑 BLOCKED: 5

Top security concerns:
  - unwrap() on user input: 12 proposals
  - unsafe{} without justification: 5 proposals
  - Missing input validation: 6 proposals

Session complete. Run /review-staged when ready for final decisions.
```
