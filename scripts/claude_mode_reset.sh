#!/bin/bash
# Claude Mode: Reset
# Unlock all files (implementation and tests)

echo "Resetting file permissions..."

# Unlock all Rust implementation files
find src/rules/cert_c -type f -name "*_c.rs" -exec chmod 644 {} \;

# Unlock utility files (now in src/utility/cert_c/)
find src/utility/cert_c -type f -name "*.rs" -exec chmod 644 {} \; 2>/dev/null

# Unlock mod.rs and integration.rs files
chmod 644 src/rules/cert_c/mod.rs 2>/dev/null
chmod 644 src/rules/cert_c/integration.rs 2>/dev/null
chmod 644 src/utility/cert_c/mod.rs 2>/dev/null
chmod 644 src/utility/mod.rs 2>/dev/null

# Unlock all C test files
find src/rules/cert_c -type f -path "*/tests/*" -name "*.c" -exec chmod 644 {} \;

echo "✅ Reset complete - all files unlocked"
echo ""
echo "To enter a specific mode:"
echo "   ./scripts/claude_mode_impl_rule.sh <RULE-ID>       - Work on one rule's implementation (+ /mode-impl-rule)"
echo "   ./scripts/claude_mode_impl_rule_utils.sh <RULE-ID> - Work on rule + utilities (+ /mode-impl-rule-utils)"
echo "   ./scripts/claude_mode_test_rule.sh <RULE-ID>       - Work on one rule's tests (+ /mode-test-rule)"
echo "   ./scripts/claude_mode_impl.sh                      - Work on all implementations (+ /mode-impl)"
echo "   ./scripts/claude_mode_test.sh                      - Work on all tests (+ /mode-test)"
