## Rule-Scoped Implementation Mode

You are being prepared to work on a **SPECIFIC CERT C rule implementation**.

The architect has run `./scripts/claude_mode_impl_rule.sh <RULE-ID>` to set file permissions.

### Step 1: Auto-detect and verify permissions

**IMPORTANT: You MUST verify permissions before proceeding.**

```bash
# Extract RULE-ID from command (e.g., /mode-impl-rule ARR38-C)
RULE_ID="$1"

if [ -z "$RULE_ID" ]; then
    echo "Error: RULE_ID required"
    echo "Usage: /mode-impl-rule <RULE-ID>"
    echo "Example: /mode-impl-rule ARR38-C"
    exit 1
fi

# Find the rule directory
RULE_DIR=$(find src/rules/cert_c -type d -name "$RULE_ID" | head -1)

if [ -z "$RULE_DIR" ]; then
    echo "Error: Rule $RULE_ID not found"
    exit 1
fi

# Find the rule's implementation file
RULE_FILE=$(find "$RULE_DIR" -type f -name "*_c.rs" | head -1)

if [ -z "$RULE_FILE" ]; then
    echo "Error: No implementation file found for $RULE_ID"
    exit 1
fi

# Check if the target rule is writable
if [ -w "$RULE_FILE" ]; then
    echo "✅ $RULE_ID implementation is UNLOCKED"
else
    echo "❌ ERROR: $RULE_ID implementation is still LOCKED"
    echo ""
    echo "Architect needs to run:"
    echo "   ./scripts/claude_mode_impl_rule.sh $RULE_ID"
    exit 1
fi

# Check that other rules are locked (spot check)
OTHER_RULE=$(find src/rules/cert_c -type f -name "*_c.rs" ! -path "$RULE_DIR/*" | head -1)
if [ -n "$OTHER_RULE" ] && [ -w "$OTHER_RULE" ]; then
    echo "⚠️  WARNING: Other rules are still UNLOCKED"
    echo "Expected: Only $RULE_ID should be writable"
    echo "Found: $OTHER_RULE is also writable"
    echo ""
    echo "Architect should run:"
    echo "   ./scripts/claude_mode_impl_rule.sh $RULE_ID"
    exit 1
fi

echo "✅ Permission verification passed"
echo "   - $RULE_ID: UNLOCKED for editing"
echo "   - Other rules: LOCKED (read-only)"
```

### Step 2: Understand Your Context

You are now in **RULE-SCOPED IMPLEMENTATION MODE for $RULE_ID**:

**UNLOCKED (you can edit):**
- `$RULE_DIR/*_c.rs` - The rule's implementation
- `$RULE_DIR/*.toml` - The rule's metadata

**LOCKED (read-only):**
- All other rule implementations (282 rules)
- All test files (2,710 files)
- Utility files in `src/utility/cert_c/`
- Module files (`mod.rs`, `integration.rs`)

### Step 3: Respect Focus Boundaries

**YOU MUST ONLY EDIT FILES IN THE ASSIGNED RULE DIRECTORY.**

If you need to:
- **Reference other rules** → READ ONLY, do not edit
- **Modify utilities** → Ask architect to run:
  ```bash
  ./scripts/claude_mode_impl_rule_utils.sh $RULE_ID
  ```
- **Work on tests** → Ask architect to switch mode:
  ```bash
  ./scripts/claude_mode_test_rule.sh $RULE_ID
  ```
- **Work on different rule** → Ask architect to run:
  ```bash
  ./scripts/claude_mode_impl_rule.sh <OTHER-RULE-ID>
  ```

### Step 4: Permission Denied Handling

If you encounter "Permission denied" when trying to edit a file:

1. **STOP immediately** - This is intentional
2. **Verify you're editing the correct rule** - Check the file path
3. **If you need to edit that file** - Ask architect to adjust permissions
4. **DO NOT** try to work around the restriction

### Important Notes

- Permission restrictions are a **safety feature**, not a bug
- They prevent accidental cross-rule contamination
- If you get blocked, it means you need to ask for permission changes
- The architect will adjust permissions as needed

### Workflow Example

```
✅ Good workflow:
1. Architect runs: ./scripts/claude_mode_impl_rule.sh ARR38-C
2. You verify permissions with script above
3. You implement ARR38-C (stay in ARR/ARR38-C/ directory)
4. You commit changes
5. Done - ARR38-C implemented without touching other rules

❌ Bad workflow:
1. Architect runs: ./scripts/claude_mode_impl_rule.sh ARR38-C
2. You skip verification
3. You try to edit ARR30-C (WRONG RULE)
4. Permission denied
5. Confusion and wasted time
```

### Ready to Start

1. Run the verification script above
2. If verification passes, proceed with implementation
3. Stay focused on $RULE_ID only
4. Ask for permission changes if needed
