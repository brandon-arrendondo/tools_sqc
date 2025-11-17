#!/bin/bash
# Claude Mode: Rule-Scoped Implementation
# Locks everything except the specified rule's implementation
# Usage: ./scripts/claude_mode_impl_rule.sh <RULE-ID>

RULE_ID="$1"

if [ -z "$RULE_ID" ]; then
    echo "Error: RULE_ID required"
    echo "Usage: $0 <RULE-ID>"
    echo "Example: $0 ARR38-C"
    exit 1
fi

# Find the rule directory (handles nested CATEGORY/RULE-ID structure)
RULE_DIR=$(find src/rules/cert_c -type d -name "$RULE_ID" | head -1)

if [ -z "$RULE_DIR" ]; then
    echo "Error: Rule $RULE_ID not found in src/rules/cert_c/"
    echo ""
    echo "Available rules:"
    find src/rules/cert_c -type d -name "*-C" | sort | sed 's|.*/||' | head -10
    echo "... (showing first 10)"
    exit 1
fi

echo "Switching to RULE-SCOPED IMPLEMENTATION mode for $RULE_ID..."

# Lock ALL implementations (including the target, will unlock next)
find src/rules/cert_c -type f -name "*_c.rs" -exec chmod 444 {} \;

# Lock ALL test files
find src/rules/cert_c -type f -path "*/tests/*" -name "*.c" -exec chmod 444 {} \;

# Lock utilities
find src/utility/cert_c -type f -name "*.rs" -exec chmod 444 {} \; 2>/dev/null

# Lock mod files
chmod 444 src/rules/cert_c/mod.rs 2>/dev/null
chmod 444 src/rules/cert_c/integration.rs 2>/dev/null
chmod 444 src/utility/cert_c/mod.rs 2>/dev/null
chmod 444 src/utility/mod.rs 2>/dev/null

# Unlock ONLY the specified rule's implementation
find "$RULE_DIR" -type f -name "*_c.rs" -exec chmod 644 {} \;

# Unlock the rule's TOML (metadata)
find "$RULE_DIR" -type f -name "*.toml" -exec chmod 644 {} \; 2>/dev/null

echo "✅ Rule-scoped implementation mode active for $RULE_ID:"
echo "   - $RULE_ID implementation is UNLOCKED for editing"
echo "   - All other rule implementations are LOCKED (read-only)"
echo "   - All test files are LOCKED (read-only)"
echo "   - Utilities are LOCKED (use claude_mode_impl_rule_utils.sh to unlock)"
echo ""
echo "Run /mode-impl-rule $RULE_ID command to tell Claude"
echo ""
echo "To unlock utilities: ./scripts/claude_mode_impl_rule_utils.sh $RULE_ID"
echo "To reset: ./scripts/claude_mode_reset.sh"
