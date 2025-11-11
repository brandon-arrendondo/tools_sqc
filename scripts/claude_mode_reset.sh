#!/bin/bash
# Claude Mode: Reset
# Unlock all files (implementation and tests)

echo "Resetting file permissions..."

# Unlock all Rust implementation files
find src/rules/cert_c -type f -name "*_c.rs" -exec chmod 644 {} \;

# Unlock utility files
find src/rules/cert_c/utils -type f -name "*.rs" -exec chmod 644 {} \; 2>/dev/null

# Unlock mod.rs files
chmod 644 src/rules/cert_c/mod.rs 2>/dev/null
chmod 644 src/rules/cert_c/utils/mod.rs 2>/dev/null

# Unlock all C test files
find src/rules/cert_c -type f -path "*/tests/*" -name "*.c" -exec chmod 644 {} \;

# Unlock all Rust unit test files
find src/rules/cert_c/tests -type f -name "*.rs" -exec chmod 644 {} \; 2>/dev/null

echo "✅ Reset complete - all files unlocked"
echo ""
echo "To enter a specific mode:"
echo "   ./scripts/claude_mode_impl.sh  - Work on implementations (+ /mode-impl)"
echo "   ./scripts/claude_mode_test.sh  - Work on tests (+ /mode-test)"
