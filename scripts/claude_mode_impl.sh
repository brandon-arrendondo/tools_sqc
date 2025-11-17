#!/bin/bash
# Claude Mode: Implementation
# Lock tests (read-only), unlock implementation files

echo "Switching to IMPLEMENTATION mode..."

# Lock all C test files (*.c in tests/ directories)
find src/rules/cert_c -type f -path "*/tests/*" -name "*.c" -exec chmod 444 {} \;

# Unlock Rust implementation files (rule implementations)
find src/rules/cert_c -type f -name "*_c.rs" ! -path "*/tests/*" -exec chmod 644 {} \;

# Unlock utility files (now in src/utility/cert_c/)
find src/utility/cert_c -type f -name "*.rs" -exec chmod 644 {} \; 2>/dev/null

# Unlock mod.rs and integration.rs
chmod 644 src/rules/cert_c/mod.rs 2>/dev/null
chmod 644 src/rules/cert_c/integration.rs 2>/dev/null
chmod 644 src/utility/cert_c/mod.rs 2>/dev/null
chmod 644 src/utility/mod.rs 2>/dev/null

echo "✅ Implementation mode active:"
echo "   - Rule implementations (*.rs) are UNLOCKED for editing"
echo "   - Utility files (src/utility/cert_c/*.rs) are UNLOCKED"
echo "   - C test files (*/tests/*.c) are LOCKED (read-only)"
echo ""
echo "Run /mode-impl command to tell Claude you're in implementation mode"
echo "To work on tests instead, run: ./scripts/claude_mode_test.sh"
