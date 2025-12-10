## Gather Opinions - Distributed Autonomous Review

**Purpose:** Rapid, persona-driven analysis of STAGED proposals to gather multiple perspectives before final review.

**Mode:** Autonomous with initial persona selection

**Prerequisites:** Proposals in `AGENTS/PROPOSALS/STAGED/` awaiting review

---

### Step 0: Create Review Branch (REQUIRED)

Before starting any reviews, create a dedicated branch for this opinion-gathering session:

```bash
# Get reviewer name and create branch
REVIEWER=$(git config user.name | tr ' ' '-' | tr '[:upper:]' '[:lower:]')
DATE=$(date +%Y%m%d)
BRANCH_NAME="opinions/${REVIEWER}-${DATE}"

# Create and checkout branch
git checkout -b "$BRANCH_NAME"
echo "Created review branch: $BRANCH_NAME"
```

**Why branch?** Each opinion will be committed individually, creating an audit trail of the review process.

---

### Step 1: Select Persona & Configure Auto-Approvals (INTERACTIVE)

**PAUSE HERE** - Two things to set up before autonomous execution begins:

---

#### 1A. Select Review Persona

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

---

#### 1B. Configure Auto-Approvals (Optional - for Autonomous Execution)

To run this workflow without manual approval prompts, add these commands to `.claude/settings.local.json`:

```json
{
  "permissions": {
    "allow": [
      "Bash(git config:*)",
      "Bash(git checkout:*)",
      "Bash(git branch:*)",
      "Bash(git add:*)",
      "Bash(git commit:*)",
      "Bash(git push:*)",
      "Bash(git rev-list:*)",
      "Bash(ls:*)",
      "Bash(cat:*)",
      "Bash(grep:*)",
      "Bash(head:*)",
      "Bash(wc:*)",
      "Bash(scripts/review_helpers.sh:*)"
    ]
  }
}
```

**What these commands do:**
- `git config/checkout/branch/add/commit/push` - Branch management and committing opinions
- `ls/cat/head/wc` - Reading proposals and counting files
- `grep` - Searching for DRY violations across the repo
- `scripts/review_helpers.sh` - Recording opinions to proposal frontmatter

**Security note:** All operations are read-only except for:
- Creating a dedicated opinion branch
- Committing opinions to proposal YAML frontmatter
- Pushing the opinion branch to remote

---

**Ready to proceed?** Please provide:
1. Your chosen persona (number 1-8 or custom description)
2. Confirm auto-approvals are configured (or acknowledge you'll approve manually)

---

### Step 2: Identify Reviewer and Scan Proposals

```bash
REVIEWER=$(git config user.name | tr ' ' '-' | tr '[:upper:]' '[:lower:]')
echo "Reviewer: $REVIEWER"
echo "Persona: {SELECTED_PERSONA}"

# Count proposals awaiting review
PROPOSAL_COUNT=$(ls -1 AGENTS/PROPOSALS/STAGED/*.md 2>/dev/null | wc -l)
echo "Found $PROPOSAL_COUNT proposals in STAGED/"
```

**Starting autonomous review as {PERSONA}...**

---

### Step 3: Process Each Proposal (AUTONOMOUS)

For **EACH** proposal in STAGED, I will:

#### A. Read Complete Proposal

```bash
# Read entire proposal - ALL sections
cat AGENTS/PROPOSALS/STAGED/{PROPOSAL_FILE}.md
```

**Focus areas:**
- Read all sections: Task, Implementation Constraints, Implementation Log, Acceptance Criteria
- Let the proposal provide context - don't assume repo-specific knowledge
- Review Implementation Log for what was actually done
- Check Acceptance Criteria for completeness
- Note any red flags specific to my persona

#### B. Code Inspection

```bash
# Extract paths/commits from proposal and inspect implementation
# Focus on the specific files mentioned in the proposal
```

**Inspection should be:**
- Guided by the proposal context, and the persona that the reviewer has selected
- Neutral (as neutral as the adopted persona allows)
- Exploratory but time-bounded (max 10 minutes per proposal)

#### C. Form Opinion (Persona-Specific + Universal Analysis)

**Opinion Categories:**
- ✅ **LOOKS_GOOD** - Implementation appears complete, no issues found
- ⚠️  **NEEDS_REVIEW** - Potential issues found that need addressing
- 🛑 **BLOCKED** - Critical issues, cannot proceed

---

### UNIVERSAL ANALYSIS (ALL PERSONAS MUST PERFORM)

**Regardless of persona, ALWAYS check for:**

#### DRY (Don't Repeat Yourself) Violations

**Within the proposal's implementation:**
- Duplicated code blocks, arrays, or constants
- Similar logic that could be extracted to a helper function
- Hardcoded values that appear multiple times

**Across the repository (exploratory, time-bounded):**
- Does this proposal implement functionality similar to another proposal or feature?
- Could this method/utility benefit other implementations?
- Should this be promoted to a shared utility location?

```bash
# Quick exploratory search for similar patterns
# Example: If proposal implements a "find_function_calls" method, check if similar exists elsewhere
grep -r "similar_pattern" src/ --include="*.rs" | head -10
```

**DRY Recommendation Framework:**
- If a method is useful to 2+ implementations → suggest moving to shared utilities
- Shared utility locations: `src/utility/cert_c/` or `src/utility/`
- Note: Don't spend >1 minute on cross-repo DRY analysis per proposal

#### KISS (Keep It Simple, Stupid) Violations

- Overly complex logic where simpler alternatives exist
- Unnecessary abstractions or indirection
- Functions doing too many things

#### Error Message Clarity

- Are violation messages clear and actionable?
- Do they provide enough context for the user to understand and fix the issue?
- Are suggestions helpful and specific?

---

### Persona-Specific Checklists

**If Security Auditor:**
- [ ] No unwrap() on user/external input
- [ ] No unsafe{} blocks without justification
- [ ] Input validation present
- [ ] Error handling doesn't leak sensitive info

**If Performance Engineer:**
- [ ] Algorithmic complexity reasonable
- [ ] No unnecessary allocations in hot paths
- [ ] Efficient data structures chosen

**If Maintainability Advocate:**
- [ ] Code is readable and well-structured
- [ ] Complex logic has explanatory comments
- [ ] No magic numbers (use constants)
- [ ] Consistent with codebase patterns

**If Test Quality Reviewer:**
- [ ] Test coverage adequate for complexity
- [ ] Edge cases tested
- [ ] Error paths tested
- [ ] Test names are descriptive

**If API Designer:**
- [ ] API names are clear and consistent
- [ ] No breaking changes to public APIs
- [ ] Error types appropriate

**If Memory Safety Expert:**
- [ ] No unsafe pointer arithmetic
- [ ] Proper lifetime management
- [ ] Buffer boundaries checked

**If Generalist:**
- [ ] Implementation matches task description
- [ ] All acceptance criteria met
- [ ] Code quality acceptable

---

#### D. Record Opinion and Commit

```bash
# Add opinion to proposal frontmatter
scripts/review_helpers.sh add-opinion \
  "AGENTS/PROPOSALS/STAGED/{PROPOSAL_FILE}.md" \
  "{SELECTED_PERSONA}" \
  "{OPINION}" \
  "{COMMENT}"

# Commit this individual opinion
git add "AGENTS/PROPOSALS/STAGED/{PROPOSAL_FILE}.md"
git commit -m "opinion({PROPOSAL_ID}): {OPINION} by {REVIEWER} as {PERSONA}"
```

**Comment Guidelines:**
- Be specific: cite line numbers, function names, file paths
- Be constructive: suggest fixes, not just problems
- Provide enough context for a deeper dive into specifics
- Note any cross-repo DRY concerns with specific file references
- Keep comments brief but actionable (5 sentences max), but if there are egregious violations you can add more

**Example comments:**
```
# Good - specific and actionable
"DRY: pointer_funcs array duplicated at lines 142/198. Similar array exists in ARR38-C (line 55). Consider shared constant in src/utility/cert_c/. Error messages are clear."

# Bad - vague
"Some duplication found. Looks okay otherwise."
```

#### E. Track Progress

```bash
echo "✓ [{CURRENT}/{TOTAL}] {PROPOSAL_ID} - {OPINION}"
```

---

### Step 4: Generate Summary Report

After processing all proposals:

```
=== Gather Opinions Summary ===
Reviewer: {REVIEWER_NAME}
Persona: {SELECTED_PERSONA}
Branch: opinions/{REVIEWER}-{DATE}
Date: {YYYY-MM-DD}

Processed: {TOTAL} proposals
Commits: {TOTAL} (one per opinion)

Opinions:
  ✅ LOOKS_GOOD: {COUNT} proposals
  ⚠️  NEEDS_REVIEW: {COUNT} proposals
  🛑 BLOCKED: {COUNT} proposals

Universal Concerns Found:
  - DRY violations (within implementation): {COUNT}
  - DRY violations (cross-repo candidates): {COUNT}
  - KISS violations: {COUNT}
  - Error message clarity issues: {COUNT}

Persona-Specific Concerns:
  - {Issue 1}: {COUNT} proposals
  - {Issue 2}: {COUNT} proposals

Cross-Repo DRY Candidates (potential shared utilities):
  - {Pattern/Method}: Found in {PROPOSAL_1}, {PROPOSAL_2}
  - {Pattern/Method}: Found in {PROPOSAL_3}, {PROPOSAL_4}

Next Steps:
  - Push branch: git push -u origin opinions/{REVIEWER}-{DATE}
  - Create PR or wait for architect to run /review-staged
```

---

### Step 5: Finalize Session

```bash
# Push the opinion branch
git push -u origin "$BRANCH_NAME"

# Summary
echo "Opinion gathering complete."
echo "Branch: $BRANCH_NAME"
echo "Commits: $(git rev-list --count main..$BRANCH_NAME)"
```

---

### Important Guidelines

**DO:**
- Create branch before starting
- Commit each opinion individually
- Read the COMPLETE proposal (let it provide context)
- Apply persona lens consistently while staying neutral
- Be exploratory for DRY violations (but time-bounded)
- Check for cross-repo patterns that could become shared utilities
- Be specific in comments (line numbers, file paths, similar files)
- Record opinion even if "LOOKS_GOOD"

**DON'T:**
- Skip the branch creation step
- Batch commits (each opinion = one commit)
- Assume repo-specific knowledge (let proposal guide you)
- Spend >5 minutes per proposal (rapid screening)
- Search every nook and cranny for DRY (be exploratory, not exhaustive)
- Modify code (this is read-only review)
- Make final decisions (that's Phase 2 /review-staged)

**Time Management:**
- Target: 2-4 minutes per proposal
- DRY cross-repo check: ~30-60 seconds max
- If going over: focus on Implementation Log + Acceptance Criteria only
- Deep analysis happens in Phase 2 (by architect)

---

### Exit Conditions

**Session Complete When:**
- All proposals in STAGED/ have your opinion recorded
- Each opinion has been committed individually
- Summary report generated
- Branch pushed to remote

**Next Steps:**
- Wait for other team members to complete their opinions (on their own branches)
- Architect runs `/review-staged` to synthesize all opinions and make final decisions

---

### Example Session Flow

```
User: /gather-opinions

Claude: === Step 1: Select Persona & Configure Auto-Approvals ===

PAUSE - Two things to set up before autonomous execution:

1A. Which persona should I adopt? [Shows 8 options]

1B. For autonomous execution, add to .claude/settings.local.json:
    [Shows JSON config]

Ready to proceed? Please provide:
1. Your chosen persona
2. Confirm auto-approvals configured (or will approve manually)

User: Maintainability Advocate, auto-approvals configured

Claude: Creating review branch...
Created: opinions/tristan-vanfossen-20251210

Starting review as Maintainability Advocate...
Reviewer: tristan-vanfossen
Found 93 proposals in STAGED/

Processing P1-API00-C...
  - Reading proposal... (context acquired)
  - Inspecting implementation...
  - DRY check (within): OK
  - DRY check (cross-repo): Found similar pattern in P2-API05-C
  - KISS check: OK
  - Error messages: Clear
  - Persona check: No magic numbers, good structure
  ✓ [1/93] NEEDS_REVIEW - committed

Processing P1-API01-C...
  ✓ [2/93] LOOKS_GOOD - committed
...

=== Summary ===
Branch: opinions/tristan-vanfossen-20251210
Processed: 93 proposals
Commits: 93

Cross-Repo DRY Candidates:
  - find_call_expression(): P1-API00-C, P2-API05-C, P1-FIO37-C
  - is_modification_function(): P2-ENV30-C, P1-STR31-C

Session complete.
```
