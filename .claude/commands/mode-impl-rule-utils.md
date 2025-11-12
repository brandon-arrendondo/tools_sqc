## Rule-Scoped Implementation Mode + Utilities

This is the same as `/mode-impl-rule` but with **utility files also unlocked**.

The architect has run `./scripts/claude_mode_impl_rule_utils.sh <RULE-ID>`.

### What You Can Edit

**UNLOCKED (you can edit):**
- `$RULE_DIR/*_c.rs` - The rule's implementation
- `$RULE_DIR/*.toml` - The rule's metadata
- `src/utility/cert_c/*.rs` - Utility files
- `src/utility/cert_c/mod.rs` - Utility module file
- `src/utility/mod.rs` - Utility root module

**LOCKED (read-only):**
- All other rule implementations (282 rules)
- All test files (2,710 files)
- Rule module files (`src/rules/cert_c/mod.rs`, `integration.rs`)

### Verification

Run the same verification script from `/mode-impl-rule`, plus:

```bash
# Additionally check utilities are writable
UTIL_FILE=$(find src/utility/cert_c -type f -name "*.rs" | head -1)
if [ -n "$UTIL_FILE" ] && [ ! -w "$UTIL_FILE" ]; then
    echo "❌ ERROR: Utility files are still LOCKED"
    echo ""
    echo "Architect needs to run:"
    echo "   ./scripts/claude_mode_impl_rule_utils.sh $RULE_ID"
    exit 1
fi

echo "✅ Utility files are UNLOCKED"
```

### When to Use This Mode

Use this mode when implementing a rule that requires:
- Creating new utility functions
- Modifying existing utility functions
- Sharing code between multiple rules

### Important

- **Be careful with utilities** - Changes affect ALL rules
- **Test thoroughly** - Utility changes can break other rules
- **Document well** - Other rules depend on utilities
- **Consider scope** - If only one rule needs it, keep it in the rule

### Workflow

1. Start with `/mode-impl-rule` (utilities locked)
2. If you discover you need to modify utilities, STOP
3. Ask architect to run: `./scripts/claude_mode_impl_rule_utils.sh $RULE_ID`
4. Verify utilities are unlocked
5. Proceed with implementation + utility changes
