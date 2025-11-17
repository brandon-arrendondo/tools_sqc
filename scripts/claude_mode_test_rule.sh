#!/bin/bash
# Claude Mode: Rule-Scoped Test
# Locks everything except the specified rule's test files
# Usage: ./scripts/claude_mode_test_rule.sh <RULE-ID>

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

echo "Switching to RULE-SCOPED TEST mode for $RULE_ID..."

# Lock ALL implementations
find src/rules/cert_c -type f -name "*_c.rs" -exec chmod 444 {} \;

# Lock ALL test files (including target, will unlock next)
find src/rules/cert_c -type f -path "*/tests/*" -name "*.c" -exec chmod 444 {} \;

# Lock utilities
find src/utility/cert_c -type f -name "*.rs" -exec chmod 444 {} \; 2>/dev/null
chmod 444 src/utility/cert_c/mod.rs 2>/dev/null
chmod 444 src/utility/mod.rs 2>/dev/null

# Lock mod files
chmod 444 src/rules/cert_c/mod.rs 2>/dev/null
chmod 444 src/rules/cert_c/integration.rs 2>/dev/null

# Unlock ONLY the specified rule's test files
find "$RULE_DIR/tests" -type f -name "*.c" -exec chmod 644 {} \; 2>/dev/null

echo "✅ Rule-scoped test mode active for $RULE_ID:"
echo "   - $RULE_ID test files are UNLOCKED for editing"
echo "   - All other test files are LOCKED (read-only)"
echo "   - All implementations are LOCKED (read-only)"
echo ""
echo "Run /mode-test-rule $RULE_ID command to tell Claude"
echo "To reset: ./scripts/claude_mode_reset.sh"
