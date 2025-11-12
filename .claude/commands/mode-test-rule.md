## Rule-Scoped Test Mode

You are being prepared to work on **test cases for a SPECIFIC CERT C rule**.

The architect has run `./scripts/claude_mode_test_rule.sh <RULE-ID>`.

### Step 1: Auto-detect and verify permissions

**IMPORTANT: You MUST verify permissions before proceeding.**

```bash
# Extract RULE-ID from command (e.g., /mode-test-rule ARR38-C)
RULE_ID="$1"

if [ -z "$RULE_ID" ]; then
    echo "Error: RULE_ID required"
    echo "Usage: /mode-test-rule <RULE-ID>"
    echo "Example: /mode-test-rule ARR38-C"
    exit 1
fi

# Find the rule directory
RULE_DIR=$(find src/rules/cert_c -type d -name "$RULE_ID" | head -1)

if [ -z "$RULE_DIR" ]; then
    echo "Error: Rule $RULE_ID not found"
    exit 1
fi

# Check if the rule has a tests directory
TESTS_DIR="$RULE_DIR/tests"
if [ ! -d "$TESTS_DIR" ]; then
    echo "Error: No tests directory found for $RULE_ID at $TESTS_DIR"
    exit 1
fi

# Find a test file in the target rule
TARGET_TEST=$(find "$TESTS_DIR" -type f -name "*.c" | head -1)

if [ -z "$TARGET_TEST" ]; then
    echo "Note: No existing test files found for $RULE_ID"
    echo "You can create new test files in:"
    echo "   - $TESTS_DIR/fail/"
    echo "   - $TESTS_DIR/pass/"
else
    # Check if target test is writable
    if [ -w "$TARGET_TEST" ]; then
        echo "✅ $RULE_ID test files are UNLOCKED"
    else
        echo "❌ ERROR: $RULE_ID test files are still LOCKED"
        echo ""
        echo "Architect needs to run:"
        echo "   ./scripts/claude_mode_test_rule.sh $RULE_ID"
        exit 1
    fi
fi

# Check that other rules' tests are locked (spot check)
OTHER_TEST=$(find src/rules/cert_c -type f -path "*/tests/*.c" ! -path "$RULE_DIR/*" | head -1)
if [ -n "$OTHER_TEST" ] && [ -w "$OTHER_TEST" ]; then
    echo "⚠️  WARNING: Other test files are still UNLOCKED"
    echo "Expected: Only $RULE_ID tests should be writable"
    echo "Found: $OTHER_TEST is also writable"
    echo ""
    echo "Architect should run:"
    echo "   ./scripts/claude_mode_test_rule.sh $RULE_ID"
    exit 1
fi

# Check that implementations are locked
IMPL_FILE=$(find src/rules/cert_c -type f -name "*_c.rs" | head -1)
if [ -n "$IMPL_FILE" ] && [ -w "$IMPL_FILE" ]; then
    echo "⚠️  WARNING: Implementation files are still UNLOCKED"
    echo "Expected: All implementations should be LOCKED in test mode"
    echo ""
    echo "Architect should run:"
    echo "   ./scripts/claude_mode_test_rule.sh $RULE_ID"
    exit 1
fi

echo "✅ Permission verification passed"
echo "   - $RULE_ID tests: UNLOCKED for editing"
echo "   - Other tests: LOCKED (read-only)"
echo "   - All implementations: LOCKED (read-only)"
```

### Step 2: Understand Your Context

You are now in **RULE-SCOPED TEST MODE for $RULE_ID**:

**UNLOCKED (you can edit):**
- `$TESTS_DIR/fail/*.c` - Test cases that should trigger violations
- `$TESTS_DIR/pass/*.c` - Test cases that should NOT trigger violations

**LOCKED (read-only):**
- All other test files (2,709 files from other rules)
- All implementations (283 rules)
- Utility files

### Step 3: Test File Structure

Test files should be placed in:
- `$TESTS_DIR/fail/` - Code that VIOLATES the rule
- `$TESTS_DIR/pass/` - Code that COMPLIES with the rule

Example structure:
```
src/rules/cert_c/ARR/ARR38-C/tests/
├── fail/
│   ├── wiki_noncompliant_1.c
│   ├── wiki_noncompliant_2.c
│   └── custom_edge_case.c
└── pass/
    ├── wiki_compliant_1.c
    ├── wiki_compliant_2.c
    └── custom_safe_usage.c
```

### Step 4: Respect Focus Boundaries

**YOU MUST ONLY EDIT TEST FILES IN THE ASSIGNED RULE DIRECTORY.**

If you need to:
- **Reference other rules' tests** → READ ONLY, do not edit
- **Modify the implementation** → Ask architect to switch mode:
  ```bash
  ./scripts/claude_mode_impl_rule.sh $RULE_ID
  ```
- **Work on different rule's tests** → Ask architect to run:
  ```bash
  ./scripts/claude_mode_test_rule.sh <OTHER-RULE-ID>
  ```

### Step 5: Testing Workflow

1. Create or modify test files in `$TESTS_DIR/fail/` or `$TESTS_DIR/pass/`
2. Rebuild to regenerate integration tests: `cargo build`
3. Run tests for this rule: `cargo test $RULE_ID`
4. Verify test results match expectations
5. Commit test files

### Important Notes

- Test files are C source files (`.c`)
- build.rs generates Rust integration tests from these C files
- Test framework expects violations in `fail/` and no violations in `pass/`
- Keep tests focused and minimal (one concept per test file)

### Ready to Start

1. Run the verification script above
2. If verification passes, proceed with test creation/modification
3. Stay focused on $RULE_ID tests only
4. Follow test naming conventions (descriptive names)
