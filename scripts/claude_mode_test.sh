#!/bin/bash
# Claude Mode: Test
# Lock implementation files (read-only), unlock test files

echo "Switching to TEST mode..."

# Lock all Rust implementation files
find src/rules/cert_c -type f -name "*_c.rs" ! -path "*/tests/*" -exec chmod 444 {} \;

# Lock utility files (now in src/utility/cert_c/)
find src/utility/cert_c -type f -name "*.rs" -exec chmod 444 {} \; 2>/dev/null

# Lock mod.rs and integration.rs
chmod 444 src/rules/cert_c/mod.rs 2>/dev/null
chmod 444 src/rules/cert_c/integration.rs 2>/dev/null
chmod 444 src/utility/cert_c/mod.rs 2>/dev/null
chmod 444 src/utility/mod.rs 2>/dev/null

# Unlock all C test files
find src/rules/cert_c -type f -path "*/tests/*" -name "*.c" -exec chmod 644 {} \;

echo "✅ Test mode active:"
echo "   - C test files (*/tests/*.c) are UNLOCKED for editing"
echo "   - Rule implementations (*.rs) are LOCKED (read-only)"
echo "   - Utility files (src/utility/cert_c/*.rs) are LOCKED (read-only)"
echo ""
echo "Run /mode-test command to tell Claude you're in test mode"
echo "To work on implementations instead, run: ./scripts/claude_mode_impl.sh"
