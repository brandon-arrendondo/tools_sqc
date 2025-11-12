# P2-ALL-REMAINING-RULES-MASTER - Non-High-Priority CERT C Rules

**Status:** BACKLOG (Master Tracking Proposal)
**Priority:** P2 (Medium - post-high-priority work)
**Created:** 2025-11-12
**Architect:** Pending
**Estimated Effort:** TBD (depends on priority decisions)

## Purpose

This is a **master tracking proposal** for all non-high-priority CERT C rules (priority < P18). It provides:
1. Complete inventory of all 263 remaining rules
2. Implementation status overview
3. Tracking for future implementation work

**This proposal is in BACKLOG** because:
- High-priority rules (P18+) must be completed first
- These 263 rules are lower priority (P1-P17)
- They will be worked after the 21 high-priority rules are complete

---

## Rule Priority Definition

**CERT C Priority Levels:**
- **P1-P17**: Lower priority rules (this proposal tracks these)
- **P18+**: High priority rules (tracked in individual ACTIVE proposals)

Priority calculated as: **Severity × Likelihood**
- Severity: Low/Medium/High
- Likelihood: Unlikely/Probable/Likely

---

## Overall Status

**Total Non-High-Priority Rules:** 263
- **Priority Range:** P1 to P17
- **Implementation Status:** TBD (requires analysis)

**Work Order:**
1. Complete all 21 high-priority rules first (see individual P1-* proposals in ACTIVE)
2. Then return to this proposal for next phase planning

---

## Rules Inventory

### Summary by Priority Band

| Priority Range | Count | Description |
|---------------|-------|-------------|
| P12-P17 | TBD | Medium priority |
| P6-P11 | TBD | Medium-low priority |
| P1-P5 | TBD | Low priority |

**Detailed Analysis:** TBD (will be performed after high-priority rules complete)

---

## High-Priority Rules (NOT in this proposal)

**The following 21 rules are HIGH PRIORITY (P18+) and have individual proposals:**

1. API01-C (P18) - [P1-API01-C-implementation.md](../ACTIVE/P1-API01-C-implementation.md)
2. API02-C (P18) - [P1-API02-C-implementation.md](../ACTIVE/P1-API02-C-implementation.md)
3. ERR33-C (P27) - [P1-ERR33-C-implementation.md](../ACTIVE/P1-ERR33-C-implementation.md)
4. EXP15-C (P27) - [P1-EXP15-C-implementation.md](../ACTIVE/P1-EXP15-C-implementation.md)
5. EXP34-C (P18) - [P1-EXP34-C-implementation.md](../ACTIVE/P1-EXP34-C-implementation.md)
6. FIO30-C (P18) - [P1-FIO30-C-implementation.md](../ACTIVE/P1-FIO30-C-implementation.md)
7. FIO34-C (P18) - [P1-FIO34-C-implementation.md](../ACTIVE/P1-FIO34-C-implementation.md)
8. FIO37-C (P18) - [P1-FIO37-C-implementation.md](../ACTIVE/P1-FIO37-C-implementation.md)
9. INT18-C (P18) - [P1-INT18-C-implementation.md](../ACTIVE/P1-INT18-C-implementation.md)
10. INT32-C (P18) - [P1-INT32-C-implementation.md](../ACTIVE/P1-INT32-C-implementation.md)
11. MSC32-C (P18) - [P1-MSC32-C-implementation.md](../ACTIVE/P1-MSC32-C-implementation.md)
12. POS30-C (P18) - [P1-POS30-C-implementation.md](../ACTIVE/P1-POS30-C-implementation.md)
13. POS36-C (P18) - [P1-POS36-C-implementation.md](../ACTIVE/P1-POS36-C-implementation.md)
14. POS37-C (P18) - [P1-POS37-C-implementation.md](../ACTIVE/P1-POS37-C-implementation.md)
15. POS54-C (P27) - [P1-POS54-C-implementation.md](../ACTIVE/P1-POS54-C-implementation.md)
16. PRE09-C (P18) - [P1-PRE09-C-implementation.md](../ACTIVE/P1-PRE09-C-implementation.md)
17. SIG30-C (P18) - [P1-SIG30-C-implementation.md](../ACTIVE/P1-SIG30-C-implementation.md)
18. SIG31-C (P18) - [P1-SIG31-C-implementation.md](../ACTIVE/P1-SIG31-C-implementation.md)
19. STR38-C (P18) - [P1-STR38-C-implementation.md](../ACTIVE/P1-STR38-C-implementation.md)
20. WIN01-C (P18) - [P1-WIN01-C-implementation.md](../ACTIVE/P1-WIN01-C-implementation.md)
21. WIN02-C (P18) - [P1-WIN02-C-implementation.md](../ACTIVE/P1-WIN02-C-implementation.md)

---

## Implementation Approach

### Phase 1: High-Priority Rules First (Current)
**Work on 21 individual high-priority proposals in ACTIVE folder**
- Each has its own detailed proposal
- Tracked individually
- Must complete before moving to Phase 2

### Phase 2: Analyze Remaining Rules (Future)
**After high-priority rules complete:**
1. Analyze all 263 remaining rules
2. Categorize by priority (P12-P17, P6-P11, P1-P5)
3. Identify which have implementations vs stubs
4. Assess test coverage
5. Create prioritization plan

### Phase 3: Implement in Priority Order (Future)
**Work through remaining rules systematically:**
- May batch similar rules together
- May create sub-proposals for categories
- Will follow same quality standards as high-priority rules

---

## Progress Tracking

**High-Priority Rules:** 0/21 complete (see individual proposals)

**Remaining Rules:** Not yet started (blocked by high-priority work)

---

## Timeline

**Current Phase:** Working on 21 high-priority individual proposals

**This Proposal Becomes Active:** After all 21 high-priority rules complete

**Estimated Start Date:** TBD (depends on high-priority completion rate)

---

## Dependencies

**Blocked By:**
- All 21 high-priority rule proposals must be COMPLETE

**Blocks:**
- Full CERT C compliance certification
- 100% rule coverage

---

## Acceptance Criteria

- [ ] All 21 high-priority rules complete (see individual proposals)
- [ ] Detailed analysis of 263 remaining rules performed
- [ ] Prioritization plan created for remaining rules
- [ ] Implementation roadmap defined

---

## Architect Decisions Needed

1. **When to start?** After all 21 high-priority rules, or can start in parallel?
2. **Batching strategy:** Should similar rules be grouped together?
3. **Quality bar:** Same standards as high-priority, or different?
4. **Resource allocation:** How many rules can be worked simultaneously?

---

## Notes

- This proposal is intentionally light on detail
- Detailed planning deferred until high-priority work completes
- Prevents premature optimization and planning overhead
- Will be fleshed out when high-priority work nears completion

---

## Verification

@architect: [Pending - activate after high-priority rules complete]
