# P2-AUTO-REGISTRY - Automate mod.rs registry generation in build.rs

**Status:** BACKLOG
**Priority:** P2 (Medium - Developer efficiency improvement)
**Created:** 2025-11-12
**Category:** Build System
**Architect:** Pending
**Estimated Effort:** 8-16 hours

---

## Problem Statement

Currently, every new rule implementation requires **manual registration** in `src/rules/cert_c/mod.rs`:
1. Add `#[path = "..."] pub mod rule_name;` declaration
2. Add `registry.register(Box::new(rule_name::RuleName));` in `RuleRegistry::new()`

With 284 CERT C rules, this means 568 manual edits (2 per rule). This is:
- **Tedious:** Repetitive boilerplate for every rule
- **Error-prone:** Easy to forget registration or make typos
- **Inconsistent:** Manual ordering can become messy
- **Scalability issue:** Adding more rule sets (MISRA, etc.) multiplies the problem

---

## Current State

**Manual Registration Example:**
```rust
// src/rules/cert_c/mod.rs (currently ~115 lines, will grow to ~600+ lines)

#[path = "API/API01-C/api01_c.rs"]
pub mod api01_c;

#[path = "API/API02-C/api02_c.rs"]
pub mod api02_c;
// ... 282 more module declarations

impl RuleRegistry {
    pub fn new() -> Self {
        let mut registry = Self { rules: Vec::new() };

        registry.register(Box::new(api01_c::Api01C));
        registry.register(Box::new(api02_c::Api02C));
        // ... 282 more registrations

        registry
    }
}
```

**Build System Capability:**
- `build.rs` already auto-generates integration tests
- `build.rs` already walks rule directories to find TOML files
- `build.rs` already generates `rules-all.toml` from individual TOMLs

---

## Proposed Solution

**Auto-generate `src/rules/cert_c/mod.rs` during build:**

### Design Approach

1. **Discovery Phase** (in `build.rs`):
   - Walk `src/rules/cert_c/` directory tree
   - Find all `*.rs` files in rule subdirectories (e.g., `API/API01-C/api01_c.rs`)
   - Extract rule ID and struct name from file path conventions

2. **Generation Phase** (in `build.rs`):
   - Generate `mod.rs` with all `#[path]` declarations
   - Generate `RuleRegistry::new()` with all registrations
   - Write to `src/rules/cert_c/mod_generated.rs`
   - Include via `mod mod_generated;` in hand-written `mod.rs`

3. **Convention Requirements**:
   - Rule files must follow naming convention: `{RULE-ID}/{rule_id_lowercase}.rs`
   - Struct name must be PascalCase version of rule ID: `Api01C`, `Arr00C`, etc.
   - Can use TOML metadata to verify rule exists before including

### Implementation Plan

**Phase 1: Add generation function to build.rs (4-6 hours)**
```rust
fn generate_cert_c_registry() -> Result<()> {
    let cert_c_dir = PathBuf::from("src/rules/cert_c");
    let output = cert_c_dir.join("mod_generated.rs");

    // Walk directory, find all rule implementations
    // Generate module declarations and registrations
    // Write to mod_generated.rs
}
```

**Phase 2: Update mod.rs to use generated code (1-2 hours)**
```rust
// src/rules/cert_c/mod.rs (reduced to ~20 lines)
mod mod_generated;
pub use mod_generated::*;

// Keep only hand-written utility code here
```

**Phase 3: Test and verify (2-4 hours)**
- Ensure all 26 currently-implemented rules still register
- Verify new rules auto-register without manual intervention
- Test with missing files, invalid names, etc.

**Phase 4: Documentation (1-2 hours)**
- Update developer docs with naming conventions
- Add comments to build.rs explaining the generation logic

---

## Acceptance Criteria

- [ ] `build.rs` contains `generate_cert_c_registry()` function
- [ ] New rules auto-register without manual `mod.rs` edits
- [ ] All 26 currently-implemented rules still work
- [ ] Build passes: `cargo build` succeeds
- [ ] Tests pass: `cargo test --lib` succeeds
- [ ] No regressions in existing functionality
- [ ] Developer documentation updated with naming conventions

---

## Benefits

**Developer Efficiency:**
- Eliminates 2 manual edits per rule (568 total edits saved)
- New rule implementation: 4 steps → 3 steps
- Reduces cognitive load (one less thing to remember)

**Code Quality:**
- Consistent ordering (alphabetical, by category, etc.)
- Eliminates typo errors in registration
- Single source of truth (directory structure)

**Scalability:**
- Easy to add new rule sets (MISRA, CWE, custom)
- Supports hundreds or thousands of rules effortlessly

**Maintenance:**
- Less code to review in `mod.rs`
- Generated code can be excluded from code review
- Build-time errors if naming conventions violated

---

## Risks and Mitigations

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| Breaking existing builds | Low | High | Thorough testing, gradual rollout |
| Naming convention confusion | Medium | Low | Clear documentation, build-time checks |
| Build.rs complexity increase | Medium | Low | Well-commented code, separate function |
| IDE autocomplete issues | Low | Medium | Keep public API surface identical |

---

## Implementation Notes

**Naming Convention to Enforce:**
```
src/rules/cert_c/{CATEGORY}/{RULE-ID}/{rule_id_snake_case}.rs
                    ↓          ↓              ↓
Example:       API/API01-C/api01_c.rs  → pub struct Api01C
```

**Struct Name Derivation:**
```rust
// Rule ID: API01-C
// File: api01_c.rs
// Module: api01_c
// Struct: Api01C (PascalCase, hyphens removed)
```

**Generated Code Template:**
```rust
// AUTO-GENERATED by build.rs - DO NOT EDIT
// Generated: 2025-11-12 17:30:00

#[path = "API/API01-C/api01_c.rs"]
pub mod api01_c;

// ... all other modules

impl RuleRegistry {
    pub fn new() -> Self {
        let mut registry = Self { rules: Vec::new() };

        registry.register(Box::new(api01_c::Api01C));
        // ... all other registrations

        registry
    }
}
```

---

## Alternative Approaches Considered

**Alternative 1: Macro-based registration**
- Use Rust macros to declare and register rules
- Rejected: Less explicit, harder to debug

**Alternative 2: Runtime discovery via directory scan**
- Scan filesystem at runtime to discover rules
- Rejected: Slower, requires unsafe dynamic loading

**Alternative 3: Keep manual registration**
- Status quo
- Rejected: Doesn't scale, error-prone

---

## Dependencies

**Requires:**
- No blockers - can implement immediately

**May Conflict With:**
- Any in-progress rule implementations (need to coordinate)

---

## Testing Strategy

**Unit Tests:**
- Test rule ID → struct name conversion logic
- Test path discovery and filtering

**Integration Tests:**
- Verify all existing rules still register
- Verify registry.get_rule() finds all rules
- Test with mock directory structures

**Regression Tests:**
- Run full test suite before/after
- Ensure no changes in test results

---

## Success Metrics

- [ ] Developer time saved: 2 minutes per rule × 284 rules = ~9.5 hours saved
- [ ] Lines of code reduced: ~500 lines removed from mod.rs
- [ ] Build time impact: <100ms increase acceptable
- [ ] Zero registration bugs in next 50 rule implementations

---

## Related Work

**Similar Patterns in Project:**
- `build.rs` already auto-generates integration tests
- `build.rs` already discovers TOML files via directory walking
- Can reuse existing discovery and generation patterns

---

## Architect Comments

@architect: [Awaiting review and approval]

---

## Priority Justification

**P2 (Medium) because:**
- Not blocking current development (manual works)
- Significant long-term efficiency gain
- Reduces technical debt
- Improves developer experience

**Not P1 because:**
- No critical bugs or blockers
- Workaround exists (manual registration)

**Not P3 because:**
- Clear ROI: 9.5 hours saved + reduced errors
- Affects every future rule implementation
