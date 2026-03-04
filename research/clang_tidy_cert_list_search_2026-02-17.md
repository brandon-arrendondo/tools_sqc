# clang-tidy CERT Checks Complete List
**Source**: WebSearch
**Date fetched**: 2026-02-17
**Search query**: clang-tidy cert checks complete list

## Content

### clang-tidy CERT C Checks (from search results)
C-specific checks (ending in -c):
1. cert-dcl03-c
2. cert-env33-c
3. cert-err33-c
4. cert-err34-c
5. cert-exp42-c
6. cert-fio38-c
7. cert-flp30-c
8. cert-int09-c
9. cert-msc24-c
10. cert-msc30-c
11. cert-msc32-c
12. cert-msc33-c
13. cert-str34-c (likely, from CERT C str rules)

### clang-tidy CERT C++ Checks (from search results)
C++-specific checks (ending in -cpp):
- cert-dcl21-cpp, cert-dcl50-cpp, cert-dcl54-cpp, cert-dcl58-cpp, cert-dcl59-cpp
- cert-err09-cpp, cert-err52-cpp, cert-err58-cpp, cert-err60-cpp, cert-err61-cpp
- cert-msc50-cpp, cert-msc51-cpp

### Summary
- **CERT C checks: ~10-13 rules** (from what's visible)
- **CERT C++ checks: ~12 rules**
- **Total CERT checks: ~22-25**
- Many clang-tidy "cert" checks are aliases for other checks (e.g., cert-dcl03-c = misc-static-assert)

### Comparison with SqC
- **SqC: 283 CERT C rules** vs clang-tidy: ~10-13 CERT C rules
- SqC has roughly **20x more CERT C coverage** than clang-tidy
- clang-tidy's strength is in broader analysis types (not just CERT)

### Source for full list
- CERTTidyModule.cpp: https://github.com/llvm-mirror/clang-tools-extra/blob/master/clang-tidy/cert/CERTTidyModule.cpp
