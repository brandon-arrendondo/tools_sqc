# clang-tidy CERT C Checks Coverage
**Source**: WebSearch
**Date fetched**: 2026-02-17
**Search query**: "clang-tidy CERT C checks coverage list number of rules benchmark"

## Content

### Search Results

1. [Clang Tidy - Defensive programming (gjbex)](https://gjbex.github.io/Defensive_programming_and_debugging/CodeValidation/StaticCodeAnalyzers/clang_tidy/)
2. [Code analysis with clang-tidy in RHEL (Red Hat)](https://developers.redhat.com/blog/2021/04/06/get-started-with-clang-tidy-in-red-hat-enterprise-linux)
3. [clang-tidy Checks list (LLVM 8)](https://bcain-llvm.readthedocs.io/projects/clang-tools-extra/en/latest/clang-tidy/checks/list/)
4. [cert-cpp-checker (GitHub)](https://github.com/ClausKlein/cert-cpp-checker)
5. [Installing Clang-Tidy 18 for C++ Security (2025)](https://markaicode.com/clang-tidy-18-install-security-checklist/)
6. [clang-tidy Checks list (LLVM 20.0.0)](https://rocm.docs.amd.com/projects/llvm-project/en/latest/LLVM/clang-tools/html/clang-tidy/checks/list.html)

### Key Findings

#### clang-tidy CERT C Checks
- Uses `cert-` prefix for SEI CERT Secure Coding Standard checks
- `-checks=cert-*` enables all CERT checks
- ~80 checks enabled by default with `clang-analyzer` prefix
- Known CERT C checks include: cert-dcl03-c, cert-env33-c, cert-err34-c, cert-fio38-c, and others
- **Exact total not stated in search results**

#### To determine exact count
- Need to run `clang-tidy -checks=cert-* -list-checks`
- Coverage varies by clang-tidy version

### Estimated Comparison with SqC
| Feature | clang-tidy | SqC |
|---------|-----------|-----|
| CERT C Rules | ~20-40 (estimated) | 283 |
| Analysis Type | AST matchers + SA | AST-only |
| Price | Free | ? |
| Integration | IDE, CI/CD | CLI |
| CERT C++ | Yes | No (C only) |
