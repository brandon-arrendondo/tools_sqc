# Cppcheck Features and CERT C Coverage
**Source**: WebSearch
**Date fetched**: 2026-02-17
**Search query**: "cppcheck features capabilities CERT C rules checks MISRA coverage 2024 2025"

## Content

### Cppcheck Editions
- **Cppcheck (open source)**: Free, focuses on undefined behavior and dangerous constructs
- **Cppcheck Premium (commercial)**: Full MISRA C:2025 + CERT C + HIS metrics

### CERT C Coverage (Cppcheck Premium)
- New CERT C checks in v25.3.0: DCL03-C, DCL04-C, PRE02-C, PRE03-C, PRE05-C, PRE06-C, PRE07-C, PRE08-C, PRE10-C, PRE12-C
- v25.8.4: Additional CERT rules + compliance accuracy improvements
- Total CERT C coverage: Not stated explicitly, but growing with each release

### MISRA Coverage
- **v25.8.0**: Full MISRA C:2025 coverage
- **v24.5.0**: All MISRA C++ 2023 rules implemented

### Analysis Capabilities
- Static analysis for C/C++
- Unique code analysis for undefined behavior detection
- Addon scripts for MISRA and CERT compliance checking
- Uses dump files for compliance analysis (newer versions: faster without misra.py)

### Pricing
- Open source version: Free
- Premium: Commercial (pricing not in search results)

### Key Takeaway for SqC
- Cppcheck Premium is the commercial variant with CERT C coverage
- Open source Cppcheck has limited CERT C support (addon-based)
- SqC with 283 CERT C rules likely has broader CERT C coverage than open-source Cppcheck
- Cppcheck Premium's CERT C coverage is growing but may not match SqC's 283 rules
