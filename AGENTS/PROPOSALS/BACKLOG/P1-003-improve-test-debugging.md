# P1-003 - Improve Test Debugging Experience

**Status:** BACKLOG
**Priority:** P1 (High)
**Created:** 2025-11-12
**Architect:** Pending
**Estimated Effort:** 16-24 hours

## Problem Statement

When tests fail (currently 339 failing tests out of 1,199 running), developers face significant friction in debugging:

1. **Test code is generated** - Cannot set breakpoints or add debug prints easily
2. **Test source is in 3 locations** - .c file, .toml metadata, generated .rs wrapper
3. **Test names are long** - `test_arr38_c_fail_wiki_noncompliant_example_42` doesn't indicate what was tested
4. **No source location in output** - Test failure doesn't show which .c file or line
5. **Regeneration on every build** - Debug prints get overwritten

**Current Test Failure Output:**
```
test rules::cert_c::integration::generated_tests::test_arr38_c_fail_testcases_complex_expression_1 ... FAIL
```

**What Developer Needs to Know:**
- Which .c file contains this test?
- What is the test supposed to check?
- What was the actual vs expected behavior?
- Where in the .c file did the violation occur (line number)?
- What's the relationship to CERT C wiki example?

## Current State

**Test Generation Process:**
1. Build.rs scans `src/rules/cert_c/CATEGORY/RULE-ID/tests/{pass,fail}/`
2. Finds .c files: `testcases_*.c`, `wiki_*.c`
3. Generates Rust test wrapper in `target/debug/build/sqc-*/out/tests/RULE-ID_tests.rs`
4. Generated tests compile and check the C file for violations

**Generated Test Structure (Simplified):**
```rust
#[test]
fn test_arr38_c_fail_testcases_complex_expression_1() {
    let result = analyze_c_file("src/rules/cert_c/ARR/ARR38-C/tests/fail/testcases_complex_expression_1.c");
    assert!(result.has_violations(), "Expected violations but found none");
}
```

**Problems:**
- No context in test name beyond file path encoding
- No link back to source .c file in failure output
- No TOML metadata included (description, CERT reference)
- Developer must manually find .c file to understand test

## Proposed Solution

### Enhancement 1: Add Source Location Comments (2-4 hours)

Add rich comments to generated tests:

```rust
/// Test: ARR38-C - Guarantee that library functions do not form invalid pointers
/// Source: src/rules/cert_c/ARR/ARR38-C/tests/fail/testcases_complex_expression_1.c
/// Type: Fail test (should detect violation)
/// Description: Tests array pointer arithmetic with complex expressions
/// CERT Reference: https://wiki.sei.cmu.edu/confluence/display/c/ARR38-C
#[test]
fn test_arr38_c_fail_testcases_complex_expression_1() {
    const TEST_FILE: &str = "src/rules/cert_c/ARR/ARR38-C/tests/fail/testcases_complex_expression_1.c";
    const RULE_ID: &str = "ARR38-C";
    const TEST_TYPE: &str = "fail";

    let result = analyze_c_file(TEST_FILE);
    assert!(
        result.has_violations(),
        "Expected {} to detect violations in {} but found none. \
         Review test file at: {}",
        RULE_ID, TEST_FILE, TEST_FILE
    );
}
```

**Benefits:**
- IDE autocomplete shows full documentation
- Failure messages include file path
- Developer can click path to open .c file
- CERT reference linked for context

### Enhancement 2: Test Metadata Registry (4-6 hours)

Create a runtime-accessible test registry:

```rust
// Generated in build.rs
pub static TEST_METADATA: &[TestMetadata] = &[
    TestMetadata {
        test_name: "test_arr38_c_fail_testcases_complex_expression_1",
        rule_id: "ARR38-C",
        source_file: "src/rules/cert_c/ARR/ARR38-C/tests/fail/testcases_complex_expression_1.c",
        test_type: TestType::Fail,
        description: "Tests array pointer arithmetic with complex expressions",
        cert_url: "https://wiki.sei.cmu.edu/confluence/display/c/ARR38-C",
    },
    // ... 2,710 entries
];
```

**Benefits:**
- CLI tool can query metadata: `cargo run -- test-info test_arr38_c_fail_*`
- Can generate HTML test report with clickable links
- Test summary can include richer context
- Can filter tests by rule, type, source

### Enhancement 3: CLI Test Helper Tool (4-8 hours)

Add subcommand to binary:

```bash
# Find test by partial name
$ cargo run -- test find arr38

# Show test details
$ cargo run -- test show test_arr38_c_fail_testcases_complex_expression_1
Test: test_arr38_c_fail_testcases_complex_expression_1
Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
Source: src/rules/cert_c/ARR/ARR38-C/tests/fail/testcases_complex_expression_1.c
Type: Fail (should detect violation)
Description: Tests array pointer arithmetic with complex expressions
CERT: https://wiki.sei.cmu.edu/confluence/display/c/ARR38-C

Last Result: FAIL
Reason: Expected violation not detected

Run test:
  cargo test test_arr38_c_fail_testcases_complex_expression_1

View source:
  cat src/rules/cert_c/ARR/ARR38-C/tests/fail/testcases_complex_expression_1.c

# Show all failing tests for a rule
$ cargo run -- test failures ARR38-C

# Generate test report
$ cargo run -- test report --format=html > test_report.html
```

**Benefits:**
- Quickly find test source without manual directory navigation
- See test context without running it
- Filter and analyze test results
- Generate reports for stakeholders

### Enhancement 4: Persistent Debug Mode (4-6 hours)

Allow adding debug information without losing it on regeneration:

**Option A: Debug annotations in .c files**
```c
// @debug: Print AST for this expression
// @breakpoint: Pause analysis here
int *ptr = array + size + 1;  // Violation should be detected here
```

Build.rs parses these and generates corresponding debug code.

**Option B: Debug config file**
```toml
# debug.toml
[tests.arr38_c_fail_testcases_complex_expression_1]
verbose = true
print_ast = true
breakpoints = [15, 23]
```

Build.rs reads this and injects debug code into generated tests.

**Benefits:**
- Debug information persists across rebuilds
- Can enable/disable debugging per test
- Doesn't pollute generated code with manual edits

## Implementation Plan

### Phase 1: Design & Architecture (2-3 hours)
- [ ] Review current build.rs test generation code
- [ ] Design TestMetadata structure
- [ ] Design CLI interface
- [ ] Choose debug mode approach (A, B, or both)
- [ ] Document design decisions

### Phase 2: Enhance Generated Test Code (3-4 hours)
- [ ] Modify build.rs to generate rich doc comments
- [ ] Add source file constants to each test
- [ ] Improve assertion messages with context
- [ ] Extract metadata from TOML files
- [ ] Test generated code compiles and works

### Phase 3: Implement Test Metadata Registry (4-6 hours)
- [ ] Define TestMetadata struct
- [ ] Generate static registry in build.rs
- [ ] Add serialization (JSON/TOML output)
- [ ] Create accessor functions
- [ ] Test registry is complete and accurate

### Phase 4: Implement CLI Tool (4-8 hours)
- [ ] Add `test` subcommand to main.rs
- [ ] Implement `test find` command
- [ ] Implement `test show` command
- [ ] Implement `test failures` command
- [ ] Implement `test report` command (text & HTML)
- [ ] Add help text and examples

### Phase 5: Persistent Debug Mode (4-6 hours)
- [ ] Implement chosen debug approach
- [ ] Test debug annotations survive rebuild
- [ ] Document how to use debug features
- [ ] Create examples

### Phase 6: Documentation (2-3 hours)
- [ ] Update README with test debugging workflow
- [ ] Add DEBUGGING-TESTS.md guide
- [ ] Document CLI tool usage
- [ ] Add examples to CONTRIBUTING.md

## Acceptance Criteria

- [ ] Generated tests have rich doc comments with source location
- [ ] Test failures show source file path in assertion message
- [ ] Test metadata registry accessible via CLI
- [ ] CLI `test` subcommand works for find/show/failures/report
- [ ] Debug annotations persist across rebuilds
- [ ] Developer can find and debug failing test in <2 minutes
- [ ] Documentation complete
- [ ] Tests pass: `cargo test`

## Risks and Mitigations

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| Registry size bloat | Medium | Low | 2,710 entries ~50KB, acceptable |
| Build time increase | Low | Low | Metadata generation is fast |
| Complexity creep | Medium | Medium | Keep CLI tool simple, iterate |
| Debug mode misuse | Low | Low | Clear documentation, examples |

## Cost/Benefit Analysis

**Costs:**
- Development time: 16-24 hours
- Slightly larger generated code (doc comments)
- Small runtime registry (~50KB)
- Maintenance of CLI tool

**Benefits:**
- **Faster debugging:** 2 min to find test vs 10-15 min manual search
- **Better error messages:** Immediate context in failures
- **Developer satisfaction:** Less friction = happier developers
- **Onboarding:** New developers can understand tests quickly
- **Reporting:** Can generate stakeholder reports easily
- **Time savings:** 339 failing tests × 8 min savings = 45 hours saved immediately

**ROI:** Very high. This is a force multiplier for test-driven development.

## Alternatives Considered

### Alternative A: Keep tests in Rust, not C files
**Rejected:** Violates architecture of using CERT C wiki test cases. .c files are the source of truth.

### Alternative B: Manual test file index
**Rejected:** Would get out of sync. Generated registry is always accurate.

### Alternative C: Use test harness framework (criterion, etc.)
**Rejected:** Over-engineered for current needs. Custom solution is simpler.

### Alternative D: Just document where tests are
**Rejected:** Doesn't solve the friction problem. Developers need tooling, not just docs.

## Dependencies

- Build.rs enhancements (already in progress)
- TOML metadata (already exists)
- No external dependencies required

## Related Proposals

- **P0-001 (Warnings):** Clean build makes test output clearer
- **P1-001 (TOML Validation):** Ensures metadata is reliable for registry

## Example CLI Session

```bash
# Developer encounters failing test
$ cargo test arr38
...
test test_arr38_c_fail_testcases_complex_expression_1 ... FAIL

# What is this test?
$ cargo run -- test show test_arr38_c_fail_testcases_complex_expression_1
Test: test_arr38_c_fail_testcases_complex_expression_1
Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
Source: src/rules/cert_c/ARR/ARR38-C/tests/fail/testcases_complex_expression_1.c
Type: Fail test (should detect violation)
Description: Tests array pointer arithmetic with complex expressions
CERT: https://wiki.sei.cmu.edu/confluence/display/c/ARR38-C

# View the test source
$ cat src/rules/cert_c/ARR/ARR38-C/tests/fail/testcases_complex_expression_1.c
[... C code ...]

# Enable debugging for this test
$ echo 'verbose = true' >> debug.toml
$ cargo test test_arr38_c_fail_testcases_complex_expression_1 -- --nocapture
[... detailed AST output ...]

# Fixed! See all ARR38-C test results
$ cargo run -- test failures ARR38-C
ARR38-C: 15 passed, 35 failed, 0 ignored
Failing tests:
  - test_arr38_c_fail_testcases_complex_expression_1
  - test_arr38_c_fail_testcases_complex_expression_2
  ...
```

## Architect Comments

@architect: [Pending review and approval]

**Questions for Architect:**
1. CLI subcommand in main binary, or separate `sqc-test-tool` binary?
2. Preference for debug mode: annotations in .c files vs config file?
3. Should test report HTML generation be included, or just text output?
4. Priority: All enhancements, or just source location comments first?

---

## Implementation Log

[To be updated during implementation]

---

## Adversarial Review

[To be completed when moved to STAGED]

---

## Verification

@architect: [Pending]
