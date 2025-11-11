## Implementation Mode Workflow

You are being prepared to work on CERT C rule implementations.

### Step 1: Auto-detect and set permissions

First, check the current file permissions and run the appropriate script if needed:

```bash
# Check if we need to switch modes
if [ -w "src/rules/cert_c/ARR/ARR30-C/tests/fail/wiki_noncompliant_1.c" ]; then
    echo "Test files are unlocked - switching to implementation mode..."
    ./scripts/claude_mode_impl.sh
else
    echo "Already in implementation or reset mode"
fi
```

### Step 2: Context

You are now in **IMPLEMENTATION MODE**:
- **UNLOCKED**: Rule implementations (`*_c.rs`), utility files (`utils/*.rs`)
- **LOCKED**: C test files (`*/tests/*.c`), Rust unit tests (`tests/*.rs`)

### Step 3: Wait for user guidance

Now wait for the user to tell you which rule or area they want you to work on.

Examples of what to expect:
- "Implement ARR00-C"
- "Fix the bug in MEM30-C"
- "Improve the detection logic for STR31-C"
- "Add support for detecting X pattern in EXP33-C"

### File Structure Reference

Each rule follows this pattern:
```
src/rules/cert_c/CATEGORY/RULE-ID/
├── RULE-ID.yaml       # Rich metadata (read-only reference)
├── RULE-ID.toml       # Runtime config (can edit enabled flag)
├── rule_id.rs         # Implementation (EDIT THIS)
└── tests/
    ├── fail/*.c       # Should trigger violations (READ-ONLY)
    └── pass/*.c       # Should NOT trigger violations (READ-ONLY)
```

### Important Notes

- If you get "Permission denied" editing a test file, this is **expected** - tests are locked in implementation mode
- You can READ test files to understand expected behavior
- Focus on tree-sitter query patterns and violation detection logic
- To switch to test mode, user should run `./scripts/claude_mode_test.sh` and use `/mode-test`
