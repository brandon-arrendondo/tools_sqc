#!/bin/bash
# Claude Mode: Implementation
# Lock tests (read-only), unlock implementation files

echo "Switching to IMPLEMENTATION mode..."

# Lock all C test files (*.c in tests/ directories)
find src/rules/cert_c -type f -path "*/tests/*" -name "*.c" -exec chmod 444 {} \;

# Lock all Rust unit tests (*.rs in tests/ directory)
find src/rules/cert_c/tests -type f -name "*.rs" -exec chmod 444 {} \; 2>/dev/null

# Unlock Rust implementation files (rule implementations)
find src/rules/cert_c -type f -name "*_c.rs" ! -path "*/tests/*" -exec chmod 644 {} \;

# Unlock utility files
find src/rules/cert_c/utils -type f -name "*.rs" -exec chmod 644 {} \; 2>/dev/null

# Unlock mod.rs
chmod 644 src/rules/cert_c/mod.rs 2>/dev/null
chmod 644 src/rules/cert_c/utils/mod.rs 2>/dev/null

echo "✅ Implementation mode active:"
echo "   - Rule implementations (*.rs) are UNLOCKED for editing"
echo "   - Utility files (utils/*.rs) are UNLOCKED"
echo "   - C test files (*/tests/*.c) are LOCKED (read-only)"
echo "   - Rust test files (tests/*.rs) are LOCKED (read-only)"
echo ""
echo "Run /mode-impl command to tell Claude you're in implementation mode"
echo "To work on tests instead, run: ./scripts/claude_mode_test.sh"
