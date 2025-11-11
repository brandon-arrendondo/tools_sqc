## Test Mode Workflow

You are being prepared to work on test cases for CERT C rules.

### Step 1: Auto-detect and set permissions

First, check the current file permissions and run the appropriate script if needed:

```bash
# Check if we need to switch modes
if [ -w "src/rules/cert_c/ARR/ARR30-C/arr30_c.rs" ]; then
    echo "Implementation files are unlocked - switching to test mode..."
    ./scripts/claude_mode_test.sh
else
    echo "Already in test or reset mode"
fi
```

### Step 2: Context

You are now in **TEST MODE**:
- **UNLOCKED**: C test files (`*/tests/*.c`), Rust unit tests (`tests/*.rs`)
- **LOCKED**: Rule implementations (`*_c.rs`), utility files (`utils/*.rs`)

### Step 3: Wait for user guidance

Now wait for the user to tell you which tests they want you to work on.

Examples of what to expect:
- "Add more test cases for ARR30-C"
- "Create a test for edge case X in MEM31-C"
- "Fix the test file in STR30-C/tests/fail/"
- "Add Rust unit tests for INT32-C"

### File Structure Reference

**C Test Cases** (from wiki/testcases):
```
src/rules/cert_c/CATEGORY/RULE-ID/tests/
├── fail/
│   ├── wiki_*.c         # From wiki (EDIT THESE)
│   └── testcases_*.c    # From testcases repo (EDIT THESE)
└── pass/
    ├── wiki_*.c         # From wiki (EDIT THESE)
    └── testcases_*.c    # From testcases repo (EDIT THESE)
```

**Rust Unit Tests**:
```
src/rules/cert_c/tests/
└── rule_id.rs           # Unit tests for rule (EDIT THESE)
```

### Important Notes

- If you get "Permission denied" editing an implementation file, this is **expected** - implementations are locked in test mode
- You can READ implementation files to understand what patterns they detect
- Focus on creating comprehensive test coverage
- Test files should have clear headers indicating rule, source, and expected status
- To switch to implementation mode, user should run `./scripts/claude_mode_impl.sh` and use `/mode-impl`
