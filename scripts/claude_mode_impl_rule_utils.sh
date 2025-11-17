#!/bin/bash
# Claude Mode: Rule-Scoped Implementation + Utilities
# Same as claude_mode_impl_rule.sh but ALSO unlocks utility files
# Usage: ./scripts/claude_mode_impl_rule_utils.sh <RULE-ID>

RULE_ID="$1"

if [ -z "$RULE_ID" ]; then
    echo "Error: RULE_ID required"
    echo "Usage: $0 <RULE-ID>"
    echo "Example: $0 ARR38-C"
    exit 1
fi

# Run the base rule-scoped script
./scripts/claude_mode_impl_rule.sh "$RULE_ID" || exit 1

# Additionally unlock utilities
echo ""
echo "Unlocking utility files..."
find src/utility/cert_c -type f -name "*.rs" -exec chmod 644 {} \;
chmod 644 src/utility/cert_c/mod.rs 2>/dev/null
chmod 644 src/utility/mod.rs 2>/dev/null

echo "✅ Utility files are now UNLOCKED for editing"
echo ""
echo "Run /mode-impl-rule-utils $RULE_ID command to tell Claude"
