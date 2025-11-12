# P1-001 - Add TOML Validation to Build Process

**Status:** BACKLOG
**Priority:** P1 (High)
**Created:** 2025-11-12
**Architect:** Pending
**Estimated Effort:** 8-16 hours

## Problem Statement

The build.rs script generates `rules-all.toml` by merging 284 individual TOML files, but **does not validate the generated output**. This means:

1. Syntax errors in source TOML files propagate to generated file
2. Invalid TOML structure might not be caught until runtime
3. Schema violations (missing required fields, wrong types) go undetected
4. Malformed TOML can cause silent failures in the application

**Evidence of Issues:**
- Agent analysis found formatting errors: `"Useferror()rather"` (should be separate words)
- Invalid dates: `"May 20, 2025"` (in November)
- No validation that required fields exist

## Current State

**Current build.rs behavior:**
1. Read individual `RULE-ID.toml` files
2. Merge into `rules-all.toml`
3. Write merged content
4. **No validation step**

**Risks:**
- Generated TOML is syntactically invalid → runtime parse errors
- Missing required fields → application crashes or incorrect behavior
- Wrong data types → type conversion errors at runtime
- Inconsistent schema across rules → unpredictable behavior

**Current Schema (Inferred):**
```toml
[rules.CATEGORY.RULE-ID]
enabled = true/false
description = "..."
severity = "low|medium|high"
# ... other fields, not clearly documented
```

**Schema Sources (3 separate definitions):**
1. `scrapers/generate_tests_from_wiki.py` - defines what scraper generates
2. `build.rs` - parses but doesn't validate structure
3. Application code - consumes TOML, expects certain fields

## Proposed Solution

Add comprehensive TOML validation to build.rs that:
1. Validates syntax (parseable TOML)
2. Validates schema (required fields, correct types)
3. Validates semantics (e.g., dates in valid format, enums have valid values)
4. Fails build on validation errors with clear messages

### Three-Tier Validation:

**Tier 1: Syntax Validation (Parse Check)**
```rust
// After generating rules-all.toml, parse it back
let generated_content = fs::read_to_string(&output_path)?;
let parsed: toml::Value = toml::from_str(&generated_content)
    .context("Generated rules-all.toml is not valid TOML syntax")?;
```

**Tier 2: Schema Validation (Structure Check)**
```rust
// Define expected schema
#[derive(Deserialize)]
struct RulesAllToml {
    rules: HashMap<String, HashMap<String, RuleMetadata>>,
}

#[derive(Deserialize)]
struct RuleMetadata {
    enabled: bool,
    // other required fields...
}

// Deserialize to schema
let validated: RulesAllToml = toml::from_str(&generated_content)
    .context("Generated rules-all.toml does not match expected schema")?;
```

**Tier 3: Semantic Validation (Business Rules)**
```rust
// Validate each rule's metadata
for (category, rules) in validated.rules {
    for (rule_id, metadata) in rules {
        validate_rule_metadata(&category, &rule_id, &metadata)?;
    }
}

fn validate_rule_metadata(category: &str, rule_id: &str, metadata: &RuleMetadata) -> Result<()> {
    // Check rule_id format: XXX99-C
    // Check severity is valid enum value
    // Check dates are in valid format
    // Check URLs are well-formed
    // etc.
}
```

## Implementation Plan

### Phase 1: Define Canonical Schema (2-3 hours)
- [ ] Review all 284 TOML files to understand actual structure
- [ ] Review scraper Python script to see what it generates
- [ ] Review application code to see what it expects
- [ ] Create canonical schema definition in Rust
- [ ] Document schema in `docs/TOML-SCHEMA.md`

### Phase 2: Implement Tier 1 Validation (1-2 hours)
- [ ] Add parse check after generating rules-all.toml
- [ ] Return error if parse fails
- [ ] Test with intentionally malformed TOML
- [ ] Verify error messages are clear

### Phase 3: Implement Tier 2 Validation (2-4 hours)
- [ ] Define schema structs with `#[derive(Deserialize)]`
- [ ] Add deserialization validation
- [ ] Handle optional vs required fields correctly
- [ ] Test with missing fields, wrong types
- [ ] Verify error messages indicate which rule has issue

### Phase 4: Implement Tier 3 Validation (3-5 hours)
- [ ] Add semantic validation functions:
  - Rule ID format validation
  - Enum value validation (severity, etc.)
  - Date format validation
  - URL format validation
  - Cross-field consistency checks
- [ ] Provide clear error messages with fix suggestions
- [ ] Test with invalid data for each validation type

### Phase 5: Fix Existing Issues (2-4 hours)
- [ ] Run validation on current codebase
- [ ] Fix any TOML files that fail validation
- [ ] Update scraper if it generates invalid TOML
- [ ] Verify all 284 rules validate successfully

### Phase 6: Documentation (1-2 hours)
- [ ] Document TOML schema in docs/TOML-SCHEMA.md
- [ ] Add validation documentation to CONTRIBUTING.md
- [ ] Document what each validation checks
- [ ] Provide examples of valid and invalid TOML

## Acceptance Criteria

- [ ] Syntax validation: Generated TOML parses successfully
- [ ] Schema validation: All required fields present, correct types
- [ ] Semantic validation: Business rules enforced (dates, enums, etc.)
- [ ] Clear error messages: Developer can fix issues from error text
- [ ] All 284 existing rules pass validation
- [ ] Build fails on validation errors
- [ ] Documentation: Schema documented in docs/
- [ ] Tests pass: `cargo test`

## Risks and Mitigations

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| Schema too strict | Medium | High | Start lenient, tighten incrementally |
| Existing TOMLs fail | High | Medium | Budget time to fix, consider warnings first |
| Schema drift | Medium | Medium | Single source of truth, versioned schema |
| Performance impact | Low | Low | Validation is fast, runs only at build time |
| False positives | Medium | Medium | Test thoroughly, allow architect overrides |

## Cost/Benefit Analysis

**Costs:**
- Development time: 8-16 hours
- Initial cleanup: 2-4 hours to fix existing issues
- Ongoing: Must maintain schema definition
- Stricter requirements: Developers must follow schema

**Benefits:**
- **Early error detection:** Catch issues at build time, not runtime
- **Data quality:** Enforced consistency across all 284 rules
- **Clear contracts:** Schema documentation serves as spec
- **Refactoring safety:** Schema changes are validated everywhere
- **Reduced debugging:** No more "why did this rule fail to load?"
- **CI/CD confidence:** Invalid TOML can't pass build

**ROI:** High. Data quality issues are expensive to debug; validation is cheap insurance.

## Alternatives Considered

### Alternative A: Validate at runtime instead of build time
**Rejected:** Fails slowly (only when rule is accessed). Build-time validation is better.

### Alternative B: Use JSON Schema or similar
**Considered:** TOML doesn't have native schema language. Rust structs + serde is idiomatic.

### Alternative C: Separate validation tool
**Rejected:** Adds complexity. Integrating into build.rs is simpler and guaranteed to run.

### Alternative D: Warnings instead of errors
**Considered for Phase 1:** Could start with warnings, graduate to errors. Discuss with architect.

## Dependencies

- Requires: **P0-002 (Fix Silent Write Failures)** - Should write before validating
- Related: Scraper Python script may need updates if it generates invalid TOML
- Complements: Application code that consumes TOML should handle validation gracefully

## Open Questions for Architect

1. **Schema strictness:** Start strict and loosen, or start lenient and tighten?
2. **Existing invalid TOML:** Fix all before merging, or allow warnings initially?
3. **Schema versioning:** Should TOML files include schema version field?
4. **Canonical source:** Which codebase owns schema definition (scraper vs build.rs)?
5. **Optional fields:** What should be required vs optional? Need architect input.

## Architect Comments

@architect: [Pending review and approval]

@agent: QUESTION - Should we use the schema from `scrapers/generate_tests_from_wiki.py` as the canonical source, or define it fresh in Rust?

---

## Implementation Log

[To be updated during implementation]

---

## Adversarial Review

[To be completed when moved to STAGED]

---

## Verification

@architect: [Pending]
