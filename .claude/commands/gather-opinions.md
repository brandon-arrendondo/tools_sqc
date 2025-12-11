## Gather Opinions - Distributed Autonomous Review

**Purpose:** Rapid, persona-driven analysis of STAGED proposals to gather multiple perspectives before final review.

**Mode:** Autonomous with initial persona selection

**Prerequisites:** Proposals in `AGENTS/PROPOSALS/STAGED/` awaiting review

---

### Step 1: Select Persona (INTERACTIVE)

**PAUSE HERE** - Persona selection required before branch creation.

---

#### 1A. Select Review Persona

Before reviewing proposals, I need to understand what perspective you want me to adopt.

**Available Personas:** (located in `AGENTS/PERSONAS/`)

```bash
ls AGENTS/PERSONAS/*.md
```

| Persona File | Focus |
|--------------|-------|
| `security-auditor.md` | Vulnerabilities, unsafe code, input validation |
| `performance-engineer.md` | Algorithmic complexity, resource usage |
| `maintainability-advocate.md` | Code clarity, documentation, technical debt |
| `test-quality-reviewer.md` | Test coverage, edge cases, failure scenarios |
| `api-designer.md` | Interfaces, ergonomics, breaking changes |
| `memory-safety-expert.md` | Memory leaks, buffer overflows, lifetime issues |
| `generalist.md` | Balanced review across all dimensions |

**Custom Personas:** Create your own in `AGENTS/PERSONAS/{your-persona}.md`

After selection, read the full persona prompt:
```bash
cat AGENTS/PERSONAS/{selected-persona}.md
```

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
1. Your chosen persona file (e.g., `security-auditor` or path to custom)
2. Confirm auto-approvals are configured (or acknowledge you'll approve manually)

---

### Step 2: Create Review Branch (REQUIRED)

After persona selection, create a dedicated branch. Run these commands separately:

**Get reviewer name:**
```bash
git config user.name
```
Convert to lowercase with hyphens (e.g., "Tristan VanFossen" → "tristan-vanfossen").

**Get today's date:**
```bash
date +%Y%m%d
```

**Create the branch** (substitute actual values):
```bash
git checkout -b "opinions/{PERSONA_FILE}-{REVIEWER}-{DATE}"
```

**Example:** For persona `security-auditor`, reviewer `tristan-vanfossen`, date `20251211`:
```bash
git checkout -b "opinions/security-auditor-tristan-vanfossen-20251211"
```

**Branch format:** `opinions/{persona-file}-{reviewer}-{date}`
- Persona file basename enables workflow recovery after context compaction

**Why branch?** Each opinion will be committed individually, creating an audit trail of the review process.

---

### Step 3: Scan Proposals and Initialize Todos

**Count proposals:**
```bash
ls -1 AGENTS/PROPOSALS/STAGED/*.md | wc -l
```

**List first two proposals:**
```bash
ls -1 AGENTS/PROPOSALS/STAGED/*.md | head -2
```

**Initialize TodoWrite with exactly 5 todos:**

*Workflow steps for current proposal:*
1. `in_progress`: "Analyze {FIRST_PROPOSAL}" (read proposal + inspect implementation)
2. `pending`: "Form opinion on {FIRST_PROPOSAL}" (persona analysis + DRY/KISS checks)
3. `pending`: "Record and commit {FIRST_PROPOSAL}" (add-opinion + add-comment + git commit)

*Tracking:*
4. `pending`: "Next: {SECOND_PROPOSAL}"
5. `in_progress`: "Progress: 0/{TOTAL} reviewed"

**⚠️ CRITICAL: Do NOT modify this 5-todo structure. Do NOT batch proposals.**
This exact structure enables workflow recovery after context compaction:
- Todos 1-3 tell a resumed agent which step to continue from
- Todo 4 tells a resumed agent what proposal comes next
- Todo 5 tracks exact progress for accurate resumption
Changing this structure BREAKS recovery. Process ONE proposal at a time.

**Starting autonomous review as {PERSONA}...**

---

### Step 4: Process Each Proposal (AUTONOMOUS)

**TodoWrite Management:**

*During each proposal* - progress through steps 1→2→3:
- Mark current step `completed`, mark next step `in_progress`
- Step 1 "Analyze": covers sections A + B below
- Step 2 "Form opinion": covers section C below
- Step 3 "Record and commit": covers sections D + E below

*After completing proposal N* - reset for next proposal:
1. Create fresh workflow todos for next proposal:
   - `in_progress`: "Analyze {NEXT_PROPOSAL}"
   - `pending`: "Form opinion on {NEXT_PROPOSAL}"
   - `pending`: "Record and commit {NEXT_PROPOSAL}"
2. **Query for new next proposal** - do NOT guess filenames:
   ```bash
   ls -1 AGENTS/PROPOSALS/STAGED/*.md | head -{N+2} | tail -1
   ```
   Update to `pending`: "Next: {QUERIED_PROPOSAL}" (or remove if none remaining)
3. Update progress counter: "Progress: {N}/{TOTAL} reviewed"

**Example:** After completing proposal #2, query for proposal #4:
```bash
ls -1 AGENTS/PROPOSALS/STAGED/*.md | head -4 | tail -1
```

**Validation Checkpoint (every 10 proposals):**
After proposals 10, 20, 30, etc., verify your todo list has exactly 5 items:
- 3 workflow steps for current proposal (Analyze/Form opinion/Record and commit)
- 1 "Next:" tracking todo
- 1 "Progress:" tracking todo
If your structure has deviated (batching, different format), STOP and reset to the correct 5-todo structure before continuing.

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

**IMPORTANT:** All commands must be single-line (no `\` continuations).

```bash
# Add opinion to proposal frontmatter (creates reviewer entry)
scripts/review_helpers.sh add-opinion "AGENTS/PROPOSALS/STAGED/{PROPOSAL_FILE}.md" "{PERSONA_FILE}" "{OPINION}"

# Add comments (200 char limit per call, 5 comments max unless egregious violations)
scripts/review_helpers.sh add-comment "AGENTS/PROPOSALS/STAGED/{PROPOSAL_FILE}.md" "First observation here."
scripts/review_helpers.sh add-comment "AGENTS/PROPOSALS/STAGED/{PROPOSAL_FILE}.md" "Second observation here."

# Commit this individual opinion
git add "AGENTS/PROPOSALS/STAGED/{PROPOSAL_FILE}.md"
git commit -m "opinion({PROPOSAL_ID}): {OPINION} by {REVIEWER} as {PERSONA}"
```

**Comment Guidelines:**
- 200 character limit per comment (enforced by script)
- Be specific: cite line numbers, function names, file paths
- Be constructive: suggest fixes, not just problems
- Note any cross-repo DRY concerns with specific file references
- 5 comments max, unless egregious violations warrant more

**Example comment sequence:**
```bash
# Good - specific and actionable
scripts/review_helpers.sh add-comment "path/file.md" "DRY: pointer_funcs duplicated at lines 142/198, similar in ARR38-C line 55."
scripts/review_helpers.sh add-comment "path/file.md" "Consider shared constant in src/utility/cert_c/."
scripts/review_helpers.sh add-comment "path/file.md" "Missing test coverage for flexible array members."

# Bad - vague
scripts/review_helpers.sh add-comment "path/file.md" "Some duplication found."
```

#### E. Track Progress

```bash
echo "✓ [{CURRENT}/{TOTAL}] {PROPOSAL_ID} - {OPINION}"
```

---

### Step 5: Generate Summary Report

After processing all proposals:

```
=== Gather Opinions Summary ===
Reviewer: {REVIEWER_NAME}
Persona: {SELECTED_PERSONA}
Branch: opinions/{PERSONA_FILE}-{REVIEWER}-{DATE}
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
  - Push branch: git push -u origin opinions/{PERSONA_FILE}-{REVIEWER}-{DATE}
  - Create PR or wait for architect to run /review-staged
```

---

### Step 6: Finalize Session

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

Claude: === Step 1: Select Persona ===

PAUSE - Persona selection required before branch creation.

Available personas in AGENTS/PERSONAS/:
  - security-auditor.md
  - performance-engineer.md
  - maintainability-advocate.md
  - test-quality-reviewer.md
  - api-designer.md
  - memory-safety-expert.md
  - generalist.md

Ready to proceed? Please provide:
1. Your chosen persona file (e.g., "maintainability-advocate")
2. Confirm auto-approvals configured (or will approve manually)

User: maintainability-advocate, auto-approvals configured

Claude: Reading persona...
[Reads AGENTS/PERSONAS/maintainability-advocate.md]

Creating review branch...
Created: opinions/maintainability-advocate-tristan-vanfossen-20251210

Starting review as maintainability-advocate...
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
Branch: opinions/maintainability-advocate-tristan-vanfossen-20251210
Processed: 93 proposals
Commits: 93

Cross-Repo DRY Candidates:
  - find_call_expression(): P1-API00-C, P2-API05-C, P1-FIO37-C
  - is_modification_function(): P2-ENV30-C, P1-STR31-C

Session complete.
```
