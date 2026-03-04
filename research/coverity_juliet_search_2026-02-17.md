# Coverity Static Analysis - Juliet Benchmark
**Source**: WebSearch
**Date fetched**: 2026-02-17
**Search query**: "Coverity static analysis false positive rate Juliet benchmark detection accuracy"

## Content

### Search Results

1. [On the capability of static code analysis (Goseva-Popstojanova 2015)](https://community.wvu.edu/~kagoseva/Papers/IST-2015.pdf)
2. [JulietTestSuite/Juliet - Coverity Scan](https://scan.coverity.com/projects/juliettestsuite-juliet)
3. [Best Practices for Static Code Analysis (NCC Group PDF)](https://www.nccgroup.com/media/vegkqamt/_ncc-group-best-practices-for-static-code-aanalysis.pdf)
4. [Juliet Java - Coverity Scan](https://scan.coverity.com/projects/juliet-java)
5. [Analyzing False Positive Source Code Vulnerabilities (ResearchGate)](https://www.researchgate.net/publication/330629383)
6. [Evaluation of Static Vulnerability Detection Tools (arxiv)](https://arxiv.org/pdf/2112.04037)
7. [Coverity Prevent Final Report 2006 (CMU)](https://www.cs.cmu.edu/~aldrich/courses/654-sp07/tools/cure-coverity-06.pdf)
8. [False Positives Over Time (Chou, Coverity)](https://www.cs.umd.edu/~pugh/BugWorkshop05/papers/34-chou.pdf)
9. [Critical comparison of six static analysis tools (ScienceDirect)](https://www.sciencedirect.com/science/article/pii/S0164121222002515)

### Key Findings

#### Coverity on Juliet
- **Coverity Scan project exists for Juliet**: https://scan.coverity.com/projects/juliettestsuite-juliet
- "Does not generate any false positive errors" on cryptographic API testing (specific subset only)
- Comprehensive Juliet results not publicly available

#### Coverity FP Claims
- Coverity historically claims ~15-20% FP rate (industry marketing)
- "False Positives Over Time" paper (Chou, Coverity) discusses FP management strategies
- NCC Group best practices doc covers Coverity usage

#### Important Papers
- **"A critical comparison on six static analysis tools: Detection, agreement, and precision"** (ScienceDirect 2022)
  - Compares 6 tools on detection, agreement, precision
- **"Evaluation of Static Vulnerability Detection Tools"** (arxiv 2021)
  - Likely has quantitative comparison data

### Coverity Overview
- **Analysis Type**: Inter-procedural, path-sensitive, data-flow
- **Price**: Commercial (Synopsys) - expensive enterprise licensing
- **FP Rate**: Claims ~15-20% (marketing); specific benchmark data not public
- **CERT C**: Supports subset of CERT C rules
