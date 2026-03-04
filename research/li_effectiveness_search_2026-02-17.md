# Li / Effectiveness of Static Analysis Tools Search
**Source**: WebSearch
**Date fetched**: 2026-02-17
**Search query**: "'Li' 'how effective' static analysis tools detecting security vulnerabilities empirical study"

## Content

### Search Results

1. [An Empirical Study of Static Analysis Tools for Secure Code Review (arxiv 2407.12241)](https://arxiv.org/html/2407.12241v1)
   - Also at [ACM ISSTA 2024](https://dl.acm.org/doi/10.1145/3650212.3680313)
2. [On the capability of static code analysis to detect security vulnerabilities (PDF)](https://community.wvu.edu/~kagoseva/Papers/IST-2015.pdf)
   - Also at [ScienceDirect](https://www.sciencedirect.com/science/article/abs/pii/S0950584915001366)
3. [An Empirical Study on the Effectiveness of Static C Code Analyzers (TUM PDF)](https://mediatum.ub.tum.de/doc/1659728/1659728.pdf)
   - Also at [ResearchGate](https://www.researchgate.net/publication/362077834_An_Empirical_Study_on_the_Effectiveness_of_Static_C_Code_Analyzers_for_Vulnerability_Detection)
4. [An empirical study of security warnings from static application security testing tools (ScienceDirect)](https://www.sciencedirect.com/science/article/abs/pii/S0164121219302018)
5. [An Empirical Study on the Use of Static Analysis Tools in (PDF)](https://machiry.github.io/files/emsast.pdf)

### Key Quantitative Findings

#### 2024 Study (ISSTA - Secure Code Review, 815 real VCCs, 92 C/C++ projects, 5 tools):
- **52% of VCCs warned by a single tool** in changed functions containing vulnerable code
- **76%+ of warnings in vulnerable functions are irrelevant** to the actual vulnerability
- **22% of VCCs undetected** by any tool

#### 2015 Study (Goseva-Popstojanova - Juliet-based):
- **27% of C/C++ vulnerabilities missed by ALL three tools**
- **11% of Java vulnerabilities missed by ALL three tools**
- **41% of C/C++ vulnerabilities detected by ALL three tools**
- **21% of Java vulnerabilities detected by ALL three tools**

#### 2022 Study (TUM - Static C Code Analyzers):
- State-of-the-art tools "overlook a large number of real-world vulnerabilities"
- Most effective: CommSCA, CodeQL, and Flawfinder
- CommSCA outperformed CodeQL by 45 more bugs, Flawfinder by 26 more
- **Combining tools increases detection effectiveness by 26%**

### Key Takeaway for SqC
- No single tool achieves perfect detection
- FP rates of 76%+ in vulnerable functions are common even for top tools
- Tool combination is recommended in academia
- SqC's 56.2% FP rate is within the range seen in literature (6.5% to 76%+)
