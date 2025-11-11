#!/bin/bash
# Claude Mode: Test
# Lock implementation files (read-only), unlock test files

echo "Switching to TEST mode..."

# Lock all Rust implementation files
find src/rules/cert_c -type f -name "*_c.rs" ! -path "*/tests/*" -exec chmod 444 {} \;

# Lock utility files
find src/rules/cert_c/utils -type f -name "*.rs" -exec chmod 444 {} \; 2>/dev/null

# Lock mod.rs
chmod 444 src/rules/cert_c/mod.rs 2>/dev/null
chmod 444 src/rules/cert_c/utils/mod.rs 2>/dev/null

# Unlock all C test files
find src/rules/cert_c -type f -path "*/tests/*" -name "*.c" -exec chmod 644 {} \;

# Unlock all Rust unit test files
find src/rules/cert_c/tests -type f -name "*.rs" -exec chmod 644 {} \; 2>/dev/null

echo "✅ Test mode active:"
echo "   - C test files (*/tests/*.c) are UNLOCKED for editing"
echo "   - Rust test files (tests/*.rs) are UNLOCKED"
echo "   - Rule implementations (*.rs) are LOCKED (read-only)"
echo "   - Utility files (utils/*.rs) are LOCKED (read-only)"
echo ""
echo "Run /mode-test command to tell Claude you're in test mode"
echo "To work on implementations instead, run: ./scripts/claude_mode_impl.sh"
