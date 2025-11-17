# PROPOSALS Directory - Development Workflow Guide

## Purpose

This directory manages architectural proposals and their implementation lifecycle for the tools_scq repository. It provides a structured workflow for Claude Code (and other AI agents) to propose, implement, and validate changes under architect supervision.

## Directory Structure

```
PROPOSALS/
├── README.md          # This file - workflow documentation
├── BACKLOG/           # Proposed changes awaiting architect review
├── ACTIVE/            # Approved proposals currently being implemented
├── STALLED/           # Implementation blocked, needs architect input
├── STAGED/            # Implementation complete, awaiting adversarial review
└── COMPLETE/          # Verified and merged implementations
```

## Workflow States

### 1. BACKLOG (Proposed)
**Location:** `PROPOSALS/BACKLOG/`

**Entry Criteria:**
- Agent identifies an issue or improvement opportunity
- Agent creates a proposal markdown with analysis and implementation plan
- Proposal includes cost/benefit analysis and risk assessment

**Exit Criteria:**
- Architect reviews and approves (moves to ACTIVE)
- Architect rejects or requests changes (remains in BACKLOG with @architect comments)
- Architect deprioritizes (remains in BACKLOG indefinitely)

**Architect Actions:**
- Add `@architect: APPROVED` to approve and move to ACTIVE
- Add `@architect: CHANGES_REQUESTED - <details>` to request modifications
- Add `@architect: REJECTED - <reason>` to close without implementation
- Add `@architect: PRIORITY <P0|P1|P2|P3>` to set priority level

### 2. ACTIVE (Approved for Implementation)
**Location:** `PROPOSALS/ACTIVE/`

**Entry Criteria:**
- Architect approval marker present: `@architect: APPROVED`
- Implementation plan is clear and actionable
- Dependencies are identified and available

**Agent Responsibilities:**
- Implement the proposed changes incrementally
- Update proposal with progress notes and commits
- Document any deviations from original plan
- If blocked, move to STALLED with explanation

**Exit Criteria:**
- Implementation complete → move to STAGED
- Implementation blocked → move to STALLED with @architect marker
- Architect cancels → move back to BACKLOG or close

### 3. STALLED (Blocked, Needs Input)
**Location:** `PROPOSALS/STALLED/`

**Entry Criteria:**
- Agent encounters blocker during implementation
- Agent needs architect decision on approach
- External dependencies unavailable
- Implementation reveals unforeseen complexity

**Agent Actions:**
- Add `@architect: BLOCKED - <specific question or issue>`
- Document what was attempted and why it failed
- Propose alternative approaches if applicable
- Do NOT continue implementation until unblocked

**Architect Actions:**
- Add `@architect: UNBLOCKED - <guidance>` and move back to ACTIVE
- Add `@architect: CANCELLED - <reason>` to stop implementation
- Add `@architect: ALTERNATIVE_APPROACH - <new plan>` to redirect

### 4. STAGED (Implementation Complete, Awaiting Review)
**Location:** `PROPOSALS/STAGED/`

**Entry Criteria:**
- Agent believes implementation is complete
- All acceptance criteria met
- Tests pass (or new tests added)
- Code is ready for adversarial review

**Review Process:**
- **Agent self-review:** Agent reviews own implementation critically
- **Adversarial review:** Different agent(s) attempt to find flaws
- **Architect review:** Architect performs final verification

**Architect Actions:**
- Add `@architect: VERIFIED` and move to COMPLETE
- Add `@architect: ISSUES_FOUND - <details>` and move back to ACTIVE
- Request additional testing or documentation

### 5. COMPLETE (Verified and Merged)
**Location:** `PROPOSALS/COMPLETE/`

**Entry Criteria:**
- Architect verification complete: `@architect: VERIFIED`
- Implementation is merged or deployed
- Documentation updated
- No known issues remain

**Purpose:**
- Historical record of completed work
- Reference for future similar proposals
- Knowledge base for onboarding

## Proposal Document Template

Each proposal should be a markdown file following this structure:

```markdown
# [Proposal ID] - [Title]

**Status:** [BACKLOG|ACTIVE|STALLED|STAGED|COMPLETE]
**Priority:** [P0|P1|P2|P3]
**Created:** YYYY-MM-DD
**Architect:** [Pending|Approved|Rejected]
**Estimated Effort:** X-Y hours

## Problem Statement

[What issue does this address? Why is it a problem?]

## Current State

[Describe current behavior/architecture]

## Proposed Solution

[Detailed description of the proposed change]

## Implementation Plan

### Phase 1: [Description]
- [ ] Step 1
- [ ] Step 2
- [ ] Step 3

### Phase 2: [Description]
- [ ] Step 1
- [ ] Step 2

## Acceptance Criteria

- [ ] Criterion 1
- [ ] Criterion 2
- [ ] Tests pass
- [ ] Documentation updated

## Risks and Mitigations

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| Risk 1 | Low/Med/High | Low/Med/High | How to mitigate |

## Cost/Benefit Analysis

**Costs:**
- Development time: X hours
- Testing time: Y hours
- Maintenance burden: Z

**Benefits:**
- Benefit 1: [quantified if possible]
- Benefit 2: [quantified if possible]

## Alternatives Considered

1. **Alternative A:** [Brief description] - Rejected because [reason]
2. **Alternative B:** [Brief description] - Rejected because [reason]

## Dependencies

- Dependency 1: [description]
- Dependency 2: [description]

## Architect Comments

[This section is reserved for architect feedback]

---

## Implementation Log

### YYYY-MM-DD - [Agent Name]
[Progress update, commits, notes]

### YYYY-MM-DD - [Agent Name]
[Progress update, commits, notes]

---

## Adversarial Review

[Once in STAGED, adversarial reviewers add comments here]

---

## Verification

@architect: [APPROVED|VERIFIED|etc] - [Optional comments]
```

## Priority Levels

### P0 (Critical - Immediate Action Required)
- Blocks development or causes production issues
- Must be addressed before other work proceeds
- Target: Fix within 1-2 days
- Examples: Build failures, silent data corruption, security vulnerabilities

### P1 (High - This Quarter)
- Significantly impacts developer experience or quality
- Should be addressed soon but doesn't block work
- Target: Fix within 1-3 months
- Examples: Major pain points, technical debt with high impact

### P2 (Medium - This Year)
- Moderate impact on development or maintenance
- Can be scheduled around other priorities
- Target: Fix within 3-12 months
- Examples: Optimizations, refactoring for maintainability

### P3 (Low - Nice to Have)
- Minor improvements or enhancements
- Can be deferred indefinitely if higher priorities exist
- Target: When time permits
- Examples: Cosmetic improvements, minor optimizations

## Agent Guidelines

### Creating Proposals (BACKLOG)

1. **Research First:** Understand the current implementation thoroughly
2. **Quantify Impact:** Provide measurable costs and benefits
3. **Consider Alternatives:** Explain why this approach is best
4. **Be Specific:** Vague proposals will be rejected
5. **Include Examples:** Show concrete code or scenarios
6. **Estimate Honestly:** Better to overestimate effort than underestimate

### During Implementation (ACTIVE)

1. **Update Frequently:** Add implementation log entries daily if possible
2. **Test Incrementally:** Don't wait until the end to test
3. **Document Decisions:** Explain non-obvious choices
4. **Ask Early:** Move to STALLED before wasting time if blocked
5. **Stay Focused:** Implement only what's in the proposal, no scope creep
6. **Commit Regularly:** Small, focused commits with clear messages

### When Blocked (STALLED)

1. **Be Specific:** "I'm blocked" is not helpful; "I'm blocked because X" is
2. **Show Your Work:** Document what you tried and why it didn't work
3. **Propose Alternatives:** Give the architect options to choose from
4. **Wait for Guidance:** Do NOT guess or proceed without unblocking

### Self-Review (STAGED)

1. **Be Critical:** Look for flaws in your own implementation
2. **Test Edge Cases:** Try to break your own code
3. **Check Documentation:** Ensure README, comments are updated
4. **Verify Acceptance Criteria:** All checkboxes should be checked
5. **Run Full Test Suite:** `cargo test` and `cargo build` must pass

## Architect Guidelines

### Reviewing Proposals (BACKLOG → ACTIVE)

- Look for clear problem statements and measurable impact
- Verify alternatives were considered
- Check that implementation plan is realistic
- Ensure acceptance criteria are specific and testable
- Add `@architect: APPROVED` to approve
- Add `@architect: PRIORITY P<N>` to set priority

### Monitoring Implementation (ACTIVE)

- Check progress periodically
- Respond quickly to STALLED items (unblock agents)
- Provide guidance when implementation deviates from plan
- Cancel if no longer needed or if approach isn't working

### Final Verification (STAGED → COMPLETE)

- Review implementation against acceptance criteria
- Run tests and verify functionality
- Check code quality and documentation
- If issues found, send back to ACTIVE with specific feedback
- Add `@architect: VERIFIED` to approve and move to COMPLETE

## Special Markers

### @architect Markers
These markers are **ONLY** used by architects (human decision makers):

- `@architect: APPROVED` - Approve proposal for implementation
- `@architect: PRIORITY <P0|P1|P2|P3>` - Set priority level
- `@architect: BLOCKED - <details>` - Agent needs help/decision
- `@architect: UNBLOCKED - <guidance>` - Provide guidance to continue
- `@architect: CHANGES_REQUESTED - <details>` - Proposal needs revision
- `@architect: REJECTED - <reason>` - Do not implement
- `@architect: VERIFIED` - Implementation complete and approved
- `@architect: ALTERNATIVE_APPROACH - <plan>` - Try different approach
- `@architect: CANCELLED - <reason>` - Stop work on this proposal

**CRITICAL:** Agents must prioritize any line containing `@architect` markers. These represent direct architect guidance and override other considerations.

### Agent Markers
Agents can use these markers for communication:

- `@agent: QUESTION - <question>` - Ask for clarification
- `@agent: NOTE - <information>` - Important note for reviewers
- `@agent: WARNING - <concern>` - Potential issue identified
- `@agent: BLOCKED` - Cannot proceed, needs architect input

## Workflow Examples

### Example 1: Smooth Implementation

1. Agent creates `BACKLOG/001-fix-compiler-warnings.md`
2. Architect reviews, adds `@architect: APPROVED`, `@architect: PRIORITY P0`
3. Agent moves to `ACTIVE/001-fix-compiler-warnings.md`
4. Agent implements changes, updates proposal with commits
5. Agent self-reviews, moves to `STAGED/001-fix-compiler-warnings.md`
6. Adversarial agent reviews, finds no issues
7. Architect adds `@architect: VERIFIED`
8. Agent moves to `COMPLETE/001-fix-compiler-warnings.md`

### Example 2: Implementation Gets Blocked

1. Agent creates `BACKLOG/002-toml-validation.md`
2. Architect adds `@architect: APPROVED`, `@architect: PRIORITY P1`
3. Agent moves to `ACTIVE/002-toml-validation.md`
4. Agent discovers TOML schema is ambiguous, can't validate
5. Agent adds `@architect: BLOCKED - TOML schema has multiple interpretations, need decision on canonical format`
6. Agent moves to `STALLED/002-toml-validation.md`
7. Architect reviews, adds `@architect: UNBLOCKED - Use schema defined in scrapers/generate_tests_from_wiki.py as canonical`
8. Agent moves back to `ACTIVE/002-toml-validation.md`
9. Agent completes implementation with clarified schema
10. Agent moves to `STAGED/002-toml-validation.md`
11. Architect verifies and moves to `COMPLETE/002-toml-validation.md`

### Example 3: Changes Requested

1. Agent creates `BACKLOG/003-flatten-directories.md`
2. Architect reviews, adds `@architect: CHANGES_REQUESTED - Need cost/benefit analysis comparing nested vs flat for LLM navigation`
3. Agent updates proposal with requested analysis
4. Architect reviews again, adds `@architect: REJECTED - Analysis shows nested structure is better for LLM focus. Thanks for the analysis.`
5. Proposal remains in `BACKLOG/` as historical record of decision

## Integration with Git

Proposals should reference specific commits as they're implemented:

```markdown
## Implementation Log

### 2025-11-12 - Claude Code
- Implemented warning suppression for stub rules: `git commit abc1234`
- Added #[allow(dead_code)] to 261 stub rule files
- Verified warnings reduced from 73 to 8: `cargo build 2>&1 | grep warning | wc -l`
```

## Metrics and Tracking

Track these metrics over time:

- **Proposal velocity:** Proposals created per week
- **Implementation velocity:** Proposals moved to COMPLETE per week
- **Time in state:** Average days in BACKLOG, ACTIVE, STALLED, STAGED
- **Success rate:** % of proposals that reach COMPLETE vs REJECTED
- **Blocking rate:** % of proposals that go to STALLED

## Best Practices

### For Agents (Claude Code, etc.)

1. **One proposal, one problem:** Don't combine unrelated changes
2. **Small incremental changes:** Better than large risky changes
3. **Test before proposing:** Verify your idea works before creating proposal
4. **Document honestly:** If uncertain, say so; don't hide doubts
5. **Learn from COMPLETE:** Review past proposals for patterns

### For Architects

1. **Quick feedback:** Review BACKLOG items within 24-48 hours
2. **Clear guidance:** Be specific when requesting changes or unblocking
3. **Trust but verify:** Let agents implement, but verify thoroughly
4. **Document decisions:** Explain WHY, not just WHAT
5. **Celebrate completions:** Acknowledge good work in COMPLETE folder

## FAQ

**Q: What if I disagree with an architect decision?**
A: Add `@agent: QUESTION - <your concern>` and present your case with evidence. Ultimately, architect decisions are final.

**Q: Can I work on multiple proposals simultaneously?**
A: Yes, but clearly separate the work. Each proposal should have its own branch and commits.

**Q: What if a proposal becomes obsolete during implementation?**
A: Add `@agent: NOTE - Proposal obsolete because <reason>` and move to STALLED for architect to cancel.

**Q: How do I handle proposals that depend on each other?**
A: Document dependencies clearly. Architects will sequence approvals appropriately.

**Q: What if I find a better approach mid-implementation?**
A: Add `@agent: QUESTION - Found better approach: <description>. OK to pivot?` and wait for architect guidance.

**Q: Should I create a proposal for tiny changes (typo fixes, etc.)?**
A: No. Proposals are for architectural changes, significant refactors, or complex fixes. Trivial changes can be made directly.

## Conclusion

This workflow provides structure while maintaining flexibility. The goal is to:

1. **Capture ideas systematically** (BACKLOG)
2. **Get architect buy-in** (ACTIVE)
3. **Handle blockers gracefully** (STALLED)
4. **Validate quality** (STAGED)
5. **Build institutional knowledge** (COMPLETE)

When in doubt, over-communicate. Add notes, ask questions, document decisions. Future maintainers (including future versions of Claude Code) will thank you.

---

**Document Status:** Living document, will evolve with usage
**Last Updated:** 2025-11-12
**Maintainer:** Architecture team + Claude Code
