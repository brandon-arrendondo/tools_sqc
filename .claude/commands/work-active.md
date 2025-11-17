## Work on Active Proposals

You are now in **PROPOSAL IMPLEMENTATION MODE**. Your mission is to work through proposals in `AGENTS/PROPOSALS/ACTIVE/` one by one, implementing the changes described.

### Step 1: Scan for Active Proposals

First, check for nested subdirectories in ACTIVE:

```bash
# Check for subdirectories
ls -d AGENTS/PROPOSALS/ACTIVE/*/ 2>/dev/null
```

**If nested subdirectories exist:**
1. List all discovered subdirectories and count proposals in each
2. **ASK THE USER** which subdirectory they want to focus on
3. Work ONLY on proposals within the selected subdirectory
4. Ignore all other subdirectories and the root ACTIVE directory

```bash
# Count proposals per subdirectory (run for each discovered directory)
for dir in AGENTS/PROPOSALS/ACTIVE/*/; do
    name=$(basename "$dir")
    count=$(ls "$dir"/*.md 2>/dev/null | wc -l)
    echo "$name: $count proposals"
done
```

**After user selects their subdirectory:**
```bash
ls -1 AGENTS/PROPOSALS/ACTIVE/{SELECTED_SUBDIRECTORY}/
```

**If NO nested subdirectories exist (flat structure):**
```bash
ls -1 AGENTS/PROPOSALS/ACTIVE/*.md 2>/dev/null
```

If no proposals are found:
- Check `AGENTS/PROPOSALS/BACKLOG/` for proposals awaiting architect approval
- Inform the architect that no active proposals exist
- Wait for architect to move proposals from BACKLOG to ACTIVE

### Step 2: Read the Workflow README

Familiarize yourself with the proposal workflow:

```bash
cat AGENTS/PROPOSALS/README.md | head -100
```

**Key Points:**
- ACTIVE = Architect has approved, ready to implement
- Your job: Implement the proposal OR move to STALLED if blocked
- When complete: Move to STAGED for review
- Update proposal with implementation log as you work

### Step 3: Select Next Proposal

Pick the **highest priority** proposal from your selected subdirectory (or root ACTIVE if no subdirectories):
1. Sort by priority: P0 (critical) > P1 (high) > P2 (medium) > P3 (low)
2. Within same priority, pick oldest (FIFO)
3. If architect has specified a particular proposal, work on that one

Read the proposal thoroughly:
```bash
# For root ACTIVE (no subdirectories):
cat AGENTS/PROPOSALS/ACTIVE/{PROPOSAL_FILE}.md

# For subdirectory (when working in selected team directory):
cat AGENTS/PROPOSALS/ACTIVE/{SELECTED_SUBDIRECTORY}/{PROPOSAL_FILE}.md
```

### Step 4: Verify Architect Approval

Check the proposal contains `@architect: APPROVED`:

```bash
grep "@architect" AGENTS/PROPOSALS/ACTIVE/P0-001-*.md
```

If no approval marker found, STOP and alert architect.

### Step 5: Implement the Proposal

Follow the **Implementation Plan** section of the proposal:

**DO:**
- Work through phases sequentially
- Check off acceptance criteria as you complete them
- Test incrementally (`cargo build`, `cargo test`)
- Document your progress in the Implementation Log section
- Commit changes with clear messages referencing the proposal ID

**DO NOT:**
- Skip steps without justification
- Make changes outside the proposal scope
- Ignore test failures
- Forget to update the proposal document

**Example Implementation Log Entry:**
```markdown
## Implementation Log

### 2025-11-12 - Claude Code (via /work-active)
**Phase 1: Analysis (Completed)**
- Ran `cargo build 2>&1 | tee build_warnings.txt`
- Categorized 73 warnings:
  - 65 warnings from stub rules (can suppress)
  - 7 warnings from implemented rules (must fix)
  - 1 warning from build.rs (must fix)
- Commit: `git commit -m "P0-001: Analyze compiler warnings" abc1234`

**Phase 2: Stub Rule Suppression (In Progress)**
- Adding `#![allow(dead_code)]` to 261 stub files
- Progress: 100/261 files updated
- ...
```

### Step 6: Handle Blockers

If you encounter a blocker that prevents completion:

1. **Document the blocker clearly:**
   ```markdown
   ## Implementation Log

   ### 2025-11-12 - Claude Code
   @architect: BLOCKED - Cannot proceed: TOML schema has multiple interpretations.
   Need architect decision on canonical format:
   - Option A: Use schema from scrapers/generate_tests_from_wiki.py
   - Option B: Define new schema in build.rs
   - Option C: Merge both schemas with explicit precedence rules

   Recommend Option A (scraper as source of truth) but need confirmation.
   ```

2. **Move proposal to STALLED:**
   ```bash
   git mv AGENTS/PROPOSALS/ACTIVE/P1-001-add-toml-validation.md \
           AGENTS/PROPOSALS/STALLED/P1-001-add-toml-validation.md
   ```

3. **Commit with clear message:**
   ```bash
   git add AGENTS/PROPOSALS/STALLED/P1-001-add-toml-validation.md
   git commit -m "P1-001: STALLED - Need architect input on TOML schema"
   ```

4. **Inform architect:**
   ```
   Proposal P1-001 is STALLED. I need your input on:
   [Brief description of the blocker]

   Please review AGENTS/PROPOSALS/STALLED/P1-001-add-toml-validation.md
   and add @architect: UNBLOCKED guidance.
   ```

### Step 7: Complete Implementation

When all acceptance criteria are met:

1. **Self-review checklist:**
   - [ ] All phases of Implementation Plan completed
   - [ ] All acceptance criteria checked off
   - [ ] `cargo build` succeeds
   - [ ] `cargo test` passes (or new test failures are expected and documented)
   - [ ] Code is committed with clear messages
   - [ ] Implementation Log is complete and accurate
   - [ ] No known issues or technical debt introduced

2. **Update proposal status:**
   ```markdown
   **Status:** STAGED (awaiting adversarial review)
   ```

3. **Move to STAGED:**
   ```bash
   # From root ACTIVE:
   git mv AGENTS/PROPOSALS/ACTIVE/{PROPOSAL_FILE}.md \
           AGENTS/PROPOSALS/STAGED/{PROPOSAL_FILE}.md

   # From subdirectory:
   git mv AGENTS/PROPOSALS/ACTIVE/{SELECTED_SUBDIRECTORY}/{PROPOSAL_FILE}.md \
           AGENTS/PROPOSALS/STAGED/{PROPOSAL_FILE}.md
   ```

   **Note:** Completed proposals always move to the common STAGED directory (not into subdirectories).

4. **Commit:**
   ```bash
   git add AGENTS/PROPOSALS/STAGED/P0-001-eliminate-compiler-warnings.md
   git commit -m "P0-001: Implementation complete, ready for review"
   ```

5. **Inform architect:**
   ```
   ✅ Proposal P0-001 implementation complete!

   Summary:
   - [Brief summary of what was implemented]
   - Build status: PASSING
   - Test status: 860 passed, 339 failed (unchanged)
   - Warnings reduced: 73 → 3

   Ready for adversarial review via /review-staged
   ```

### Step 8: Continue to Next Proposal

After completing or stalling a proposal:

1. Check if more proposals are in ACTIVE
2. If yes, go to Step 3 (select next proposal)
3. If no, inform architect and wait for instructions

### Important Guidelines

**Test Frequently:**
```bash
# After each phase or major change
cargo build
cargo test --lib

# Check for new warnings
cargo build 2>&1 | grep -c "warning:"
```

**Commit Often:**
```bash
# Small, focused commits
git add <files>
git commit -m "P0-001 Phase 2: Add suppression to stub rules (100/261)"
```

**Stay Focused:**
- Implement ONLY what the proposal describes
- No scope creep
- No "while I'm here" fixes (create separate proposals)
- If you discover related issues, note them but don't fix them

**Communicate Clearly:**
- Update Implementation Log after each work session
- Use @architect markers when you need input
- Provide context: what you tried, why it didn't work, what you recommend

**Quality Over Speed:**
- Better to move to STALLED with clear blocker than to guess
- Better to take 2 hours and do it right than 30 minutes and introduce bugs
- Better to ask for clarification than to implement the wrong thing

### Error Handling

**If proposal is malformed:**
```markdown
@architect: QUESTION - This proposal is missing [X].
Should I proceed with assumptions, or do you want to revise the proposal first?
```

**If acceptance criteria are unclear:**
```markdown
@architect: QUESTION - Acceptance criterion "Tests pass" is ambiguous.
Do you mean:
A. All existing tests continue to pass (current: 860 passed, 339 failed)
B. No new test failures introduced
C. Some specific test should now pass

Assuming B, but please confirm.
```

**If implementation reveals scope is larger than estimated:**
```markdown
@architect: NOTE - Implementation is taking longer than estimated.
Original: 4-8 hours
Actual: 12 hours so far, 60% complete

Reason: [explanation]
Recommend: [continue, split into multiple proposals, or cancel]

Awaiting your guidance.
```

### Exit Conditions

**Stop working when:**
1. All ACTIVE proposals are complete or stalled
2. You encounter a critical blocker that requires architect decision
3. Build or tests are broken and you can't fix them
4. Architect explicitly tells you to stop
5. You're unsure about implementation approach (ask, don't guess)

**Do NOT:**
- Work on BACKLOG proposals (only architect moves to ACTIVE)
- Work on STALLED proposals (wait for @architect: UNBLOCKED)
- Work on STAGED proposals (those need /review-staged)
- Work on COMPLETE proposals (those are done)

### Example Session

```
Architect: /work-active

Claude: Scanning AGENTS/PROPOSALS/ACTIVE/...

Found 2 active proposals:
- P0-001-eliminate-compiler-warnings.md (Priority: P0)
- P1-002-mutex-poisoning-recovery.md (Priority: P1)

Working on highest priority: P0-001

[Reads proposal...]

Starting implementation:
Phase 1: Analyzing compiler warnings...
[Runs cargo build, categorizes warnings]
✅ Phase 1 complete

Phase 2: Adding suppressions to stub rules...
[Adds #![allow(...)] to 261 files]
✅ Phase 2 complete (reduced warnings: 73 → 8)

Phase 3: Fixing legitimate warnings...
[Fixes 7 warnings in implemented rules]
✅ Phase 3 complete (reduced warnings: 8 → 1)

Phase 4: Verification...
[Runs cargo build, cargo test]
✅ All acceptance criteria met

Self-review passed. Moving to STAGED.
[git mv ... git commit ...]

✅ P0-001 complete! Ready for /review-staged

Next proposal: P1-002-mutex-poisoning-recovery.md
Would you like me to continue, or review P0-001 first?
```

### Tips for Success

1. **Read the proposal twice** before starting
2. **Understand the "why"** not just the "what"
3. **Test after every phase** (don't wait until the end)
4. **Document as you go** (don't try to remember at the end)
5. **Ask early** if something seems wrong
6. **Commit frequently** (makes rollback easier)
7. **Stay in scope** (resist feature creep)
8. **Be honest** about blockers (don't hide them)

---

**Ready to start? First, scan for active proposals and report what you find.**
