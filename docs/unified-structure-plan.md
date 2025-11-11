# Unified Structure Plan

## Overview

This document outlines the plan to consolidate the tools_scq project structure by merging three separate metadata/testing locations into a single, unified nested structure within `src/rules/cert_c/`.

## Current Problems

1. **Fragmented Metadata**: Rule metadata exists in 3 places:
   - `scraped_docs/` - Rich YAML metadata from wiki (currently unused)
   - `rules_templates/` - Minimal TOML runtime config
   - `src/rules/cert_c/` - Rust implementations with hardcoded metadata

2. **Duplicate Test Cases**: Test examples exist in 2 places:
   - `scraped_docs/rules/cert-c/*/tests/` - Wiki-scraped examples
   - `~/tools_sqc_testcases/` - Companion repo with curated test cases

3. **Manual Registration**: Adding new rules requires manual updates in multiple places:
   - Creating TOML manifest
   - Adding Rust implementation
   - Registering in `mod.rs`
   - No automated stub generation

4. **Scattered Structure**: No clear organization linking metadata, implementation, and tests

## Unified Structure

### Final Directory Layout

```
src/rules/cert_c/
├── ARR/
│   ├── ARR00-C/
│   │   ├── ARR00-C.yaml       # Rich metadata (title, severity, CWE, etc.)
│   │   ├── ARR00-C.toml       # Runtime config (enabled, ignore_patterns)
│   │   ├── arr00_c.rs         # Implementation
│   │   └── tests/
│   │       ├── fail/*.c       # Expected violations (merged wiki + testcases)
│   │       └── pass/*.c       # Expected clean (merged wiki + testcases)
│   └── ARR30-C/
│       ├── ARR30-C.yaml
│       ├── ARR30-C.toml
│       ├── arr30_c.rs
│       └── tests/
│           ├── fail/*.c
│           └── pass/*.c
├── MEM/
│   └── MEM30-C/
│       ├── MEM30-C.yaml
│       ├── MEM30-C.toml
│       ├── mem30_c.rs
│       └── tests/
├── ... (other categories)
├── rules-all.toml             # Auto-generated from all RULE-ID.toml files
└── mod.rs                     # Updated with #[path] attributes
```

### File Roles

**RULE-ID.yaml** (e.g., ARR30-C.yaml): Rich metadata from wiki scraping
```yaml
id: ARR30-C
type: rule
category: ARR
number: 30
metadata:
  title: "Do not form or use out-of-bounds pointers"
  description: "..."
  severity: High
  likelihood: Likely
  priority: P9
  level: L2
  cert_version: "2016 Edition (Wiki)"
  last_modified: "Jul 24, 2025"
references:
  wiki: https://wiki.sei.cmu.edu/confluence/...
  cwe: [CWE-119, CWE-823, CWE-125]
  related_rules: []
```

**RULE-ID.toml** (e.g., ARR30-C.toml): Runtime configuration
```toml
[rule]
id = "ARR30-C"
category = "ARR"
enabled = true
ignore_patterns = []
```

**arr30_c.rs**: Implementation using tree-sitter
```rust
use tree_sitter::{Node, Query, QueryCursor};
use crate::violations::Violation;

pub fn check_arr30_c(source: &str, tree: &tree_sitter::Tree) -> Vec<Violation> {
    // Implementation...
}
```

**tests/**: Merged test cases
- `fail/*.c` - Should trigger violations for this specific rule
- `pass/*.c` - Should NOT trigger violations for this rule
- Merged from both wiki examples and tools_sqc_testcases repo

## Migration Phases

### Phase 1: Restructure Existing Rules ✅

**Goal**: Move 27 implemented rules to nested structure

**Steps**:
1. Create nested directories: `src/rules/cert_c/CAT/RULE-C/`
2. Move implementations to nested locations
3. Copy YAML files from `scraped_docs/` to nested locations
4. Rename `rules_templates/*.toml` to `manifest.toml` in nested locations
5. Update `mod.rs` with `#[path]` attributes for nested modules

**Example Update to mod.rs**:
```rust
#[path = "ARR/ARR00-C/arr00_c.rs"]
pub mod arr00_c;

#[path = "ARR/ARR30-C/arr30_c.rs"]
pub mod arr30_c;
```

### Phase 2: Merge Test Cases ✅

**Goal**: Consolidate wiki examples and testcases repo into single `tests/` directory

**Steps**:
1. For each implemented rule:
   - Create `tests/fail/` and `tests/pass/` directories
   - Copy wiki examples from `scraped_docs/rules/cert-c/*/tests/`
   - Copy testcases from `~/tools_sqc_testcases/RULE-C/`
   - Preserve original filenames (already descriptive)
   - Add header comments to indicate source (wiki vs testcases)

**Test File Header Format**:
```c
/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers
 * Source: wiki | testcases
 * Status: FAIL | PASS
 * Description: Brief explanation of what this test validates
 */
```

### Phase 3: Claude/LLM Protection Mechanism ✅

**Goal**: Prevent LLM from accidentally editing tests when working on implementation (and vice versa)

**Protection Scripts**:

**scripts/claude_mode_impl.sh**:
```bash
#!/bin/bash
# Lock tests, unlock implementation files
find src/rules/cert_c -type f -path "*/tests/*" -name "*.c" -exec chmod 444 {} \;
find src/rules/cert_c -type f -name "*.rs" -exec chmod 644 {} \;
echo "✅ Implementation mode: Tests locked (read-only), code unlocked"
```

**scripts/claude_mode_test.sh**:
```bash
#!/bin/bash
# Lock implementation, unlock tests
find src/rules/cert_c -type f -name "*.rs" -exec chmod 444 {} \;
find src/rules/cert_c -type f -path "*/tests/*" -name "*.c" -exec chmod 644 {} \;
echo "✅ Test mode: Code locked (read-only), tests unlocked"
```

**scripts/claude_mode_reset.sh**:
```bash
#!/bin/bash
# Unlock everything
find src/rules/cert_c -type f \( -name "*.rs" -o -name "*.c" \) -exec chmod 644 {} \;
echo "✅ Reset mode: All files unlocked"
```

**Claude Workflow Commands**:

**.claude/commands/mode-impl.md**:
```markdown
You are now in IMPLEMENTATION MODE.

Context:
- Test files (*.c) are READ-ONLY - locked by file permissions
- Rust implementation files (*.rs) are unlocked for editing
- User has run: ./scripts/claude_mode_impl.sh

Your task:
- Work on Rust rule implementations in src/rules/cert_c/
- You can read test files but cannot modify them
- Focus on making rules detect violations correctly
- If you need to edit tests, ask user to switch to test mode

If you see "Permission denied" when trying to edit a test file, this is expected - you're in the right mode.
```

**.claude/commands/mode-test.md**:
```markdown
You are now in TEST MODE.

Context:
- Rust implementation files (*.rs) are READ-ONLY - locked by file permissions
- Test files (*.c) are unlocked for editing
- User has run: ./scripts/claude_mode_test.sh

Your task:
- Work on test cases in src/rules/cert_c/*/tests/
- You can read implementations but cannot modify them
- Focus on creating comprehensive fail/pass test cases
- Ensure test headers clearly document expected behavior

If you see "Permission denied" when trying to edit a Rust file, this is expected - you're in the right mode.
```

**Usage**:
```bash
# User switches to implementation mode
./scripts/claude_mode_impl.sh
# Then runs: /mode-impl

# User switches to test mode
./scripts/claude_mode_test.sh
# Then runs: /mode-test

# User resets permissions
./scripts/claude_mode_reset.sh
```

### Phase 4: Build System Updates ✅

**Goal**: Auto-generate `rules-all.toml` from individual `RULE-ID.toml` files

**Implementation**: Create `build.rs`

```rust
use std::fs;
use std::path::Path;

fn main() {
    println!("cargo:rerun-if-changed=src/rules/cert_c");

    let cert_c_dir = Path::new("src/rules/cert_c");
    let output_path = cert_c_dir.join("rules-all.toml");

    let mut manifests = Vec::new();

    // Walk directory tree looking for RULE-ID.toml files
    for entry in walkdir::WalkDir::new(cert_c_dir)
        .follow_links(true)
        .into_iter()
        .filter_map(|e| e.ok())
    {
        let path = entry.path();
        // Match files like ARR30-C.toml, not mod.rs or other files
        if let Some(filename) = path.file_name() {
            let filename_str = filename.to_string_lossy();
            if filename_str.ends_with("-C.toml") {
                if let Ok(content) = fs::read_to_string(path) {
                    manifests.push(content);
                }
            }
        }
    }

    // Combine all manifests
    let combined = manifests.join("\n\n");

    // Write to rules-all.toml
    fs::write(output_path, combined)
        .expect("Failed to write rules-all.toml");

    println!("Generated rules-all.toml from {} manifests", manifests.len());
}
```

**Update Cargo.toml**:
```toml
[build-dependencies]
walkdir = "2.5"
```

**Update Cargo.toml modules section**:
```toml
[[bin]]
name = "sqc"
path = "src/main.rs"

[lib]
name = "tools_scq"
path = "src/lib.rs"

# Enable nested module structure
[package.metadata.docs.rs]
all-features = true
rustdoc-args = ["--document-private-items"]
```

### Phase 5: Tooling Updates ✅

**Goal**: Update invoke tasks and pre-commit hooks for new structure

#### Update tasks.py

**New `invoke test` behavior**:
```python
@task
def test(c):
    """
    Run wiki-scraped test validation.

    This validates that implemented rules correctly detect violations
    in their associated wiki-scraped test files.

    Expected behavior:
    - rules/cert-c/XXX/RULEXX/tests/fail/*.c should trigger RULEXX-C violations
    - rules/cert-c/XXX/RULEXX/tests/pass/*.c should NOT trigger RULEXX-C violations
    """
    print("=" * 60)
    print("Running wiki-scraped test validation...")
    print("=" * 60)

    # Ensure binary exists
    binary = Path("./target/release/sqc")
    if not binary.exists():
        print("❌ Binary not found. Run 'invoke build' first.")
        sys.exit(1)

    # Run analysis on scraped_docs
    scraped_dir = Path("./scraped_docs")
    if not scraped_dir.exists():
        print("❌ scraped_docs/ directory not found.")
        sys.exit(1)

    violations_csv = "violations_test.csv"

    print(f"\nRunning analysis on {scraped_dir}...")
    result = c.run(
        f"./target/release/sqc {scraped_dir} --export {violations_csv}",
        warn=True
    )

    if result.exited != 0:
        print("❌ Analysis failed!")
        sys.exit(1)

    # Parse violations and validate
    print(f"\nValidating results from {violations_csv}...")
    violations = parse_violations(violations_csv)

    validation_results = validate_wiki_tests(violations)

    # Print results
    print("\n" + "=" * 60)
    print("Test Validation Results")
    print("=" * 60)

    total_rules = len(validation_results)
    passed = sum(1 for r in validation_results.values() if r['passed'])
    failed = total_rules - passed

    for rule_id, result in sorted(validation_results.items()):
        status = "✅ PASS" if result['passed'] else "❌ FAIL"
        print(f"{status} {rule_id}")

        if not result['passed']:
            if result['missing_fail_violations']:
                print(f"  ⚠️  Expected violations in fail/ but none found:")
                for f in result['missing_fail_violations'][:3]:
                    print(f"      - {f}")

            if result['unexpected_pass_violations']:
                print(f"  ⚠️  Unexpected violations in pass/:")
                for f in result['unexpected_pass_violations'][:3]:
                    print(f"      - {f}")

    print("\n" + "=" * 60)
    print(f"Summary: {passed}/{total_rules} rules passed validation")
    print("=" * 60)

    # Clean up test CSV
    os.remove(violations_csv)

    if failed > 0:
        print(f"\n❌ {failed} rules failed validation!")
        sys.exit(1)

    print("\n✅ All implemented rules passed validation!")


@task
def test_full(c):
    """
    Run full test suite against tools_sqc_testcases.

    This runs comprehensive validation against the curated test cases
    in the companion repository.
    """
    print("=" * 60)
    print("Running full test suite (tools_sqc_testcases)...")
    print("=" * 60)

    # Ensure binary exists
    binary = Path("./target/release/sqc")
    if not binary.exists():
        print("❌ Binary not found. Run 'invoke build' first.")
        sys.exit(1)

    # Check for companion repo
    testcases_dir = Path("../tools_sqc_testcases")
    if not testcases_dir.exists():
        print("❌ tools_sqc_testcases/ directory not found.")
        print("   Expected at: ../tools_sqc_testcases/")
        sys.exit(1)

    violations_csv = "violations_full.csv"

    print(f"\nRunning analysis on {testcases_dir}...")
    result = c.run(
        f"./target/release/sqc {testcases_dir} --export {violations_csv}",
        warn=True
    )

    if result.exited != 0:
        print("❌ Analysis failed!")
        sys.exit(1)

    # Parse and validate
    print(f"\nValidating results from {violations_csv}...")
    violations = parse_violations(violations_csv)

    validation_results = validate_testcases(violations, testcases_dir)

    # Print results
    print("\n" + "=" * 60)
    print("Full Test Suite Results")
    print("=" * 60)

    total_rules = len(validation_results)
    passed = sum(1 for r in validation_results.values() if r['passed'])
    failed = total_rules - passed

    for rule_id, result in sorted(validation_results.items()):
        status = "✅ PASS" if result['passed'] else "❌ FAIL"
        print(f"{status} {rule_id}: {result['fail_detected']}/{result['fail_total']} fail, "
              f"{result['pass_clean']}/{result['pass_total']} pass")

    print("\n" + "=" * 60)
    print(f"Summary: {passed}/{total_rules} rules passed validation")
    print("=" * 60)

    # Clean up
    os.remove(violations_csv)

    if failed > 0:
        print(f"\n❌ {failed} rules failed validation!")
        sys.exit(1)

    print("\n✅ All rules passed full validation!")


def parse_violations(csv_file):
    """Parse violations CSV into structured data."""
    violations = []

    with open(csv_file, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            # Parse title: "RULE-ID:path:line version:hash"
            title = row['Title']
            parts = title.split(':')

            if len(parts) >= 2:
                rule_id = parts[0]
                file_path = parts[1]

                violations.append({
                    'rule_id': rule_id,
                    'file_path': file_path,
                    'description': row['Description']
                })

    return violations


def validate_wiki_tests(violations):
    """
    Validate wiki-scraped tests.

    Expected behavior:
    - tests/fail/*.c should trigger violations for that rule
    - tests/pass/*.c should NOT trigger violations for that rule
    """
    # Group violations by rule and file
    violations_by_rule = defaultdict(lambda: defaultdict(list))

    for v in violations:
        rule_id = v['rule_id']
        file_path = v['file_path']
        violations_by_rule[rule_id][file_path].append(v)

    # Find all rule directories with tests
    scraped_dir = Path("./scraped_docs/rules/cert-c")
    results = {}

    for category_dir in scraped_dir.iterdir():
        if not category_dir.is_dir():
            continue

        for rule_dir in category_dir.iterdir():
            if not rule_dir.is_dir():
                continue

            tests_dir = rule_dir / "tests"
            if not tests_dir.exists():
                continue

            # Extract rule ID from directory name (e.g., ARR30 -> ARR30-C)
            rule_id = f"{rule_dir.name}-C"

            fail_dir = tests_dir / "fail"
            pass_dir = tests_dir / "pass"

            fail_files = list(fail_dir.glob("*.c")) if fail_dir.exists() else []
            pass_files = list(pass_dir.glob("*.c")) if pass_dir.exists() else []

            # Check fail files - should have violations
            missing_fail = []
            for fail_file in fail_files:
                file_violations = violations_by_rule[rule_id].get(str(fail_file), [])
                if not file_violations:
                    missing_fail.append(fail_file.name)

            # Check pass files - should NOT have violations
            unexpected_pass = []
            for pass_file in pass_files:
                file_violations = violations_by_rule[rule_id].get(str(pass_file), [])
                if file_violations:
                    unexpected_pass.append(pass_file.name)

            results[rule_id] = {
                'passed': len(missing_fail) == 0 and len(unexpected_pass) == 0,
                'missing_fail_violations': missing_fail,
                'unexpected_pass_violations': unexpected_pass,
                'fail_total': len(fail_files),
                'pass_total': len(pass_files)
            }

    return results


def validate_testcases(violations, testcases_dir):
    """
    Validate companion repo test cases.

    Expected behavior:
    - RULE-ID/fail/*.c should trigger violations for that rule
    - RULE-ID/pass/*.c should NOT trigger violations for that rule
    """
    violations_by_rule = defaultdict(lambda: defaultdict(list))

    for v in violations:
        rule_id = v['rule_id']
        file_path = v['file_path']
        violations_by_rule[rule_id][file_path].append(v)

    results = {}

    for rule_dir in testcases_dir.iterdir():
        if not rule_dir.is_dir() or rule_dir.name.startswith('.'):
            continue

        rule_id = rule_dir.name

        fail_dir = rule_dir / "fail"
        pass_dir = rule_dir / "pass"

        fail_files = list(fail_dir.glob("*.c")) if fail_dir.exists() else []
        pass_files = list(pass_dir.glob("*.c")) if pass_dir.exists() else []

        fail_detected = 0
        pass_clean = 0

        for fail_file in fail_files:
            if violations_by_rule[rule_id].get(str(fail_file)):
                fail_detected += 1

        for pass_file in pass_files:
            if not violations_by_rule[rule_id].get(str(pass_file)):
                pass_clean += 1

        results[rule_id] = {
            'passed': fail_detected == len(fail_files) and pass_clean == len(pass_files),
            'fail_detected': fail_detected,
            'fail_total': len(fail_files),
            'pass_clean': pass_clean,
            'pass_total': len(pass_files)
        }

    return results
```

#### Update .pre-commit-config.yaml

**Use local invoke task hook instead of direct git hook**:
```yaml
repos:
  # Rust formatting
  - repo: https://github.com/doublify/pre-commit-rust
    rev: v1.0
    hooks:
      - id: fmt
        name: cargo fmt
        description: Format Rust code with rustfmt
        args: ['--manifest-path=Cargo.toml', '--', '--check']

      - id: cargo-check
        name: cargo check
        description: Check Rust code compiles
        args: ['--manifest-path=Cargo.toml']

  # Local hooks for invoke tasks
  - repo: local
    hooks:
      - id: invoke-build
        name: invoke build
        description: Build release binary
        entry: invoke build
        language: system
        pass_filenames: false
        always_run: true

      - id: invoke-test
        name: invoke test
        description: Run wiki-scraped test validation
        entry: invoke test
        language: system
        pass_filenames: false
        always_run: true
```

**Installation**:
```bash
# Install pre-commit framework
pip install pre-commit

# Install hooks
pre-commit install

# Run manually
pre-commit run --all-files
```

### Phase 6: Generate Stubs (AFTER validation) ⏳

**Goal**: Generate skeleton implementations for 256 unimplemented rules

**Wait until**: Structure is validated with existing 27 rules

**Script**: `scripts/generate_stubs.py`
```python
#!/usr/bin/env python3
"""Generate skeleton implementations for unimplemented rules."""

import yaml
from pathlib import Path
from typing import Dict, List

def generate_stub(rule_yaml: Path, output_dir: Path):
    """Generate a skeleton Rust implementation from rule metadata."""

    with open(rule_yaml) as f:
        metadata = yaml.safe_load(f)

    rule_id = metadata['id']
    category = metadata['category']
    title = metadata['metadata']['title']
    description = metadata['metadata']['description']

    # Generate Rust module name (ARR30-C -> arr30_c)
    module_name = rule_id.lower().replace('-', '_')

    # Create implementation file
    impl_file = output_dir / f"{module_name}.rs"

    stub_content = f'''//! {rule_id}: {title}
//!
//! {description[:200]}...
//!
//! ## References
//! - Wiki: {metadata['references']['wiki']}
//! - CWE: {', '.join(metadata['references'].get('cwe', []))}

use tree_sitter::{{Node, Query, QueryCursor}};
use crate::violations::Violation;

/// Check for violations of {rule_id}
///
/// ## Implementation Notes
/// TODO: Implement detection logic using tree-sitter queries
pub fn check_{module_name}(source: &str, tree: &tree_sitter::Tree) -> Vec<Violation> {{
    let mut violations = Vec::new();

    // TODO: Implement rule logic
    // 1. Write tree-sitter query to find relevant patterns
    // 2. Analyze matched nodes
    // 3. Create violations for confirmed issues

    violations
}}

#[cfg(test)]
mod tests {{
    use super::*;

    #[test]
    fn test_{module_name}_basic() {{
        // TODO: Add basic test case
        todo!("Implement basic test");
    }}
}}
'''

    impl_file.write_text(stub_content)
    print(f"Generated stub: {impl_file}")

def main():
    scraped_dir = Path("scraped_docs/rules/cert-c")
    src_rules_dir = Path("src/rules/cert_c")

    # Find all rule.yaml files without corresponding implementations
    for category_dir in scraped_dir.iterdir():
        if not category_dir.is_dir():
            continue

        for rule_dir in category_dir.iterdir():
            if not rule_dir.is_dir():
                continue

            rule_yaml = rule_dir / "rule.yaml"
            if not rule_yaml.exists():
                continue

            # Check if implementation exists
            category = category_dir.name
            rule_id = rule_dir.name
            impl_dir = src_rules_dir / category / rule_id

            module_name = rule_id.lower().replace('-', '_')
            impl_file = impl_dir / f"{module_name}.rs"

            if impl_file.exists():
                print(f"✓ {rule_id} already implemented")
                continue

            # Generate stub
            impl_dir.mkdir(parents=True, exist_ok=True)
            generate_stub(rule_yaml, impl_dir)

if __name__ == "__main__":
    main()
```

## Three-Tier Testing Strategy

### Tier 1: Pre-commit (Wiki Examples)
- **Scope**: Fast validation that implemented rules detect violations in their own test files
- **Location**: `src/rules/cert_c/*/tests/`
- **Command**: `invoke test` (via pre-commit hook)
- **Purpose**: Catch regressions before commit
- **Expected Runtime**: < 5 seconds

### Tier 2: Build (Companion Repo)
- **Scope**: Comprehensive validation against curated test cases
- **Location**: `../tools_sqc_testcases/`
- **Command**: `invoke test-full`
- **Purpose**: Full validation before merge
- **Expected Runtime**: < 30 seconds

### Tier 3: Full (Juliet Suite)
- **Scope**: Complete NIST test suite validation
- **Location**: TBD (Juliet test suite download)
- **Command**: `invoke test-juliet` (future)
- **Purpose**: Benchmark against industry standard
- **Expected Runtime**: Minutes

## Migration Checklist

### Preparation
- [x] Create planning document (this file)
- [ ] Create Claude protection scripts
- [ ] Update .pre-commit-config.yaml
- [ ] Update tasks.py with new validation logic
- [ ] Create build.rs for auto-generating rules-all.toml

### Structure Migration (27 Implemented Rules)
- [ ] Create nested directory structure for each implemented rule
- [ ] Move Rust implementations to nested locations
- [ ] Copy rule.yaml from scraped_docs/
- [ ] Copy/rename manifest.toml from rules_templates/
- [ ] Update mod.rs with #[path] attributes
- [ ] Verify build: `cargo build --release`

### Test Migration
- [ ] For each implemented rule, merge tests:
  - [ ] Create tests/fail/ and tests/pass/ directories
  - [ ] Copy wiki examples from scraped_docs/
  - [ ] Copy testcases from ~/tools_sqc_testcases/
  - [ ] Add source headers to test files
- [ ] Verify with invoke test

### Validation
- [ ] Run `invoke build` - should succeed
- [ ] Run `invoke test` - should pass all implemented rules
- [ ] Run `invoke test-full` - should pass comprehensive validation
- [ ] Run `pre-commit run --all-files` - should succeed
- [ ] Test Claude protection scripts

### Cleanup
- [ ] Remove old scraped_docs/ directory
- [ ] Remove old rules_templates/ directory
- [ ] Update README.md with new structure
- [ ] Document in CONTRIBUTING.md

### Future Work (256 Unimplemented Rules)
- [ ] Run `scripts/generate_stubs.py` to create skeleton implementations
- [ ] Update mod.rs to register all stubs
- [ ] Verify all stubs compile but return empty violations
- [ ] Prioritize implementation based on severity/priority

## Benefits

1. **Single Source of Truth**: All rule information in one place
2. **Easier Navigation**: Clear hierarchy (category → rule → {metadata, impl, tests})
3. **Automated Tooling**: Build system generates combined config automatically
4. **Better Testing**: Merged test cases provide comprehensive coverage
5. **LLM Protection**: File permissions prevent accidental cross-contamination
6. **Scalability**: Easy to add new rules (stub generation)
7. **Documentation**: Rich YAML metadata accessible for future tooling

## Risks and Mitigations

**Risk**: Breaking existing tooling during migration
**Mitigation**: Migrate incrementally, validate at each step

**Risk**: LLM accidentally editing wrong files
**Mitigation**: File permission protection scripts

**Risk**: Test cases conflicting between wiki and testcases repo
**Mitigation**: Keep both, add headers to distinguish source

**Risk**: Auto-generation of rules-all.toml fails
**Mitigation**: Keep manual fallback, add build.rs tests
