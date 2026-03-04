# Goseva-Popstojanova 2015 - Juliet Benchmark Study
**Source**: WebSearch
**Date fetched**: 2026-02-17
**Search query**: Goseva-Popstojanova "On the capability of static code analysis" Juliet results

## Content

### Paper Details
- **Title**: On the capability of static code analysis to detect security vulnerabilities
- **Authors**: Goseva-Popstojanova, Perhinschi
- **Published**: Information and Software Technology, 2015
- **PDF**: https://community.wvu.edu/~kagoseva/Papers/IST-2015.pdf
- **ACM**: https://dl.acm.org/doi/10.1016/j.infsof.2015.08.002

### Methodology
- **Three widely used COMMERCIAL tools** (anonymized as Tool A, B, C)
- **22 CWEs for C/C++** (21,265 test cases)
- **19 CWEs for Java** (7,516 test cases)
- Used largest Juliet subset claimed detectable by all three tools
- Metrics: accuracy, recall (Pd), probability of false alarm (Pfa)
- Statistical testing of results

### Key Results

#### C/C++ Results
- **27% of vulnerabilities missed by ALL three tools**
- **41% of vulnerabilities detected by ALL three tools**
- ~32% detected by some but not all tools

#### Java Results
- **11% of vulnerabilities missed by ALL three tools**
- **21% of vulnerabilities detected by ALL three tools**

#### Tool C (Best Performer)
- Smallest false alarm rate: mean 7% (C/C++) and 5% (Java)
- Referenced in abstract of search results

#### Cross-Tool Finding
- No statistically significant difference between tools for vulnerability detection
- Tools are somewhat interchangeable for overall detection rates

### Implications for SqC
- Even COMMERCIAL tools miss 27% of C/C++ vulnerabilities on Juliet
- Best commercial tool: 7% FP rate on C/C++ subset
- SqC's 56.2% FP rate is much higher than commercial Tool C's 7%
- BUT SqC checks 283 CERT C rules vs focused CWE subset in this study
- Broader rule coverage inherently means more FPs
