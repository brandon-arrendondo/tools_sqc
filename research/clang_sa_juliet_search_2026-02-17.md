# Clang Static Analyzer Juliet Benchmark Search
**Source**: WebSearch
**Date fetched**: 2026-02-17
**Search query**: "clang static analyzer Juliet benchmark false positive rate detection accuracy evaluation"

## Content

### Search Results

1. [Clang SA in Xcode (ResearchGate)](https://www.researchgate.net/figure/Clang-static-analyzer-integrated-in-Xcode_fig4_312938554)
2. [TUM ISSTA 2022 paper (PDF)](https://mediatum.ub.tum.de/doc/1659728/1659728.pdf)
3. [Benchmarking Static Analysis Tools for C (Medium)](https://medium.com/codex/11-static-analysis-tools-for-c-4fe5f63c18a5)
4. [Goseva-Popstojanova 2015 (PDF)](https://community.wvu.edu/~kagoseva/Papers/IST-2015.pdf)
5. [LLM inspecting static bug warnings (ACM)](https://dl.acm.org/doi/10.1145/3653718)
6. [Static Analysis with Clang (blog)](http://btorpey.github.io/blog/2015/04/27/static-analysis-with-clang/)
7. [Empirical Study on Use of Static Analysis Tools (PDF)](https://machiry.github.io/files/emsast.pdf)
8. [A Comparison of Static Analysis Tools (PDF)](https://chenweixiang.github.io/docs/A_Comparison_of_Static_Analysis_Tools_for_Vulnerability_Detection_in_C_C++_Code.pdf)
9. [CodeChecker false positives docs](https://codechecker.readthedocs.io/en/latest/analyzer/false_positives/)

### Key Findings

#### Clang SA / Infer on Juliet
- **Infer**: Detects on average 79% of vulnerabilities across 4 CWE categories on Juliet C/C++
- Some analyzers achieve 80-100% detection for certain vulnerability types
- **27% of C/C++ vulnerabilities missed by ALL three tools studied**
- **41% of C/C++ vulnerabilities detected by ALL three tools**

#### Clang SA False Positive Management
- SMT-based refutation algorithm implemented in Clang SA
- Can remove false bugs and speed up analysis
- Only 1% slowdown when unable to remove any bugs
- Taint analysis: 100% coverage on taint-related subset while suppressing all FPs

#### Analysis Depth
- Clang SA: Path-sensitive, inter-procedural analysis
- Uses symbolic execution
- Much deeper than AST-only (more like SqC's approach)

### Comparison with SqC
| Feature | Clang SA | SqC |
|---------|----------|-----|
| Analysis Type | Path-sensitive, symbolic execution | AST-only |
| Juliet Detection | ~79% (Infer, similar class) | 43.8% TP rate |
| FP Rate | Not clearly stated | 56.2% |
| CERT C Rules | Via clang-tidy checks | 283 rules |
| Price | Free | ? |
