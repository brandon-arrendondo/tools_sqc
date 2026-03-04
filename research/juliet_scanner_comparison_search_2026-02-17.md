# Using Juliet Test Suite to Compare Static Security Scanners
**Source**: WebSearch
**Date fetched**: 2026-02-17
**Search query**: "Using the Juliet Test Suite to Compare Static Security Scanners" results detection rates false positive Cppcheck Fortify

## Content

### Papers Found

1. [Using the Juliet Test Suite to Compare Static Security Scanners (ResearchGate PDF)](https://www.researchgate.net/publication/270897332_Using_the_Juliet_Test_Suite_to_Compare_Static_Security_Scanners)
2. [Same paper (JKU PDF)](https://www.se.jku.at/wp-content/uploads/2014/08/2014.Using-the-Juliet-Test-Suite.pdf)
3. [IEEE Xplore](https://ieeexplore.ieee.org/document/7509496/)
4. [ACM DL](https://dl.acm.org/doi/10.5220/0005032902440252)
5. [A Comparative Study of Static Code Analysis tools for Vulnerability Detection in C/C++ and JAVA (ScienceDirect)](https://www.sciencedirect.com/science/article/pii/S1877050920312023)
6. [Benchmarking Static Analysis Tools for C | CodeX (Medium)](https://medium.com/codex/11-static-analysis-tools-for-c-4fe5f63c18a5)
7. [A comparative Study of Static Code Analysis tools (PDF)](http://junikhyatjournal.in/no_1_dec_20/84.pdf)

### Key Quantitative Findings

#### Juliet Test Suite Details
- 64,099 test cases in 100k files
- Designed specifically for assessing SAST tool capabilities

#### Tool Performance on Juliet
- **Cppcheck**: Detected the maximum number of vulnerabilities among 3 tools studied, BUT reported the highest amount of false positives
- **Cppcheck philosophy**: "no false negatives" (may not report all errors, but tries not to report wrong errors) -- CONTRADICTS the finding above about "highest FP"
- **Flawfinder**: Identified the greatest number of vulnerability categories
- **RATS**: Similar vulnerability types to Cppcheck

#### Cross-Tool Findings (Goseva-Popstojanova 2015)
- **27% of C/C++ vulnerabilities missed by ALL three tools**
- **11% of Java vulnerabilities missed by ALL three tools**
- **41% of C/C++ vulnerabilities detected by ALL three tools**
- **21% of Java vulnerabilities detected by ALL three tools**

### Key Takeaway for SqC
- Even tools focused on "no false negatives" (Cppcheck) have significant FP issues
- Tool combination is necessary for comprehensive coverage
- The specific FP rates on Juliet aren't clearly stated numerically in these search results -- need to fetch the actual PDFs
