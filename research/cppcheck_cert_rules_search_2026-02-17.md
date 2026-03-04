# Cppcheck CERT C Rules Coverage
**Source**: WebSearch
**Date fetched**: 2026-02-17
**Search query**: "cppcheck CERT C rules detection rate number of rules supported checkers list"

## Content

### Search Results

1. [Polyspace Check SEI CERT-C++ (MATLAB)](https://www.mathworks.com/help/bugfinder/ref/checkseicertccertcpp.html)
2. [Cppcheck PlatformIO docs](https://docs.platformio.org/en/latest//advanced/static-code-analysis/tools/cppcheck.html)
3. [Cppcheck Premium 24.11.0 released](https://www.cppcheck.com/product-news/cppcheck-premium-24.11.0)
4. [Cppcheck homepage](https://www.cppcheck.com/)
5. [Cppcheck manual v2.19.0 (PDF)](https://cppcheck.sourceforge.io/manual.pdf)
6. [SourceForge: Updated CERT Rules coverage](https://sourceforge.net/p/cppcheck/discussion/general/thread/2a5e291fb1/)
7. [SourceForge: CERT-rules coverage?](https://sourceforge.net/p/cppcheck/discussion/general/thread/aa3b570771/?limit=25)
8. [SourceForge: ListOfChecks wiki](https://sourceforge.net/p/cppcheck/wiki/ListOfChecks/)
9. [Cppcheck Premium 24.2.0 released](https://www.cppcheck.com/product-news/cppcheck-premium-24.2.0-released)

### CERT C Rules in Cppcheck (Open Source - cert.py addon)
Known supported rules from cert.py addon:
- EXP05, EXP42, EXP46, EXP15
- INT31, INT34 (via shiftNegative, shiftTooManyBits)
- STR03, STR05, STR07, STR11
- ENV33
- MSC24, MSC30
- API01

**Total: ~14 rules explicitly listed** (open source version)

### Cppcheck Premium
- Supports CERT C 2016 and CERT C++ 2016 standards
- Additional CERT rules added in each premium release
- Exact total not publicly stated

### Comparison with SqC
| Feature | Cppcheck (OSS) | Cppcheck Premium | SqC |
|---------|----------------|------------------|-----|
| CERT C Rules | ~14 | Unknown (growing) | 283 |
| Price | Free | Commercial | ? |
| Analysis Type | Data flow + pattern | Data flow + pattern | AST-only |
| MISRA | Via addon | Full C:2025 | N/A |

### Key Takeaway
- Open source Cppcheck has ~14 CERT C rules -- SqC has 20x more
- CERT website mapping for Cppcheck is "quite incomplete"
- Cppcheck Premium adds more but total unknown
- SqC's 283 CERT C rules is a strong differentiator
