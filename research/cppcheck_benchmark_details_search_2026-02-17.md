# Cppcheck Benchmark Details (Medium/CodeX + Comparisons)
**Source**: WebSearch
**Date fetched**: 2026-02-17
**Search query**: site:medium.com "benchmarking static analysis tools" C Juliet cppcheck

## Content

### Key Papers/Sources Found

1. [Benchmarking Static Analysis Tools for C | CodeX (Medium)](https://medium.com/codex/11-static-analysis-tools-for-c-4fe5f63c18a5)
2. [A Comparison of Open-Source Static Analysis Tools (ResearchGate)](https://www.researchgate.net/publication/328906714)
3. [A Comparison of Static Analysis Tools (PDF)](https://chenweixiang.github.io/docs/A_Comparison_of_Static_Analysis_Tools_for_Vulnerability_Detection_in_C_C++_Code.pdf)
4. [David Wheeler - Static analysis tools for security](https://dwheeler.com/essays/static-analysis-tools.html)
5. [On the Use of Open-Source C/C++ Static Analysis Tools in Large Projects (IEEE)](https://ieeexplore.ieee.org/document/9236998/)
6. [Benchmarking static code analyzers (ScienceDirect)](https://www.sciencedirect.com/science/article/abs/pii/S0951832018304721)

### Key Findings from Medium/CodeX Benchmark

#### Tools Evaluated on Juliet (64,099 test cases, 100k files, 100+ weaknesses)
- AdLint, Clang-Tidy, CppCheck, Flawfinder, Frama-C, IKOS

#### Results Summary
- **AdLint, Clang-Tidy, CppCheck, Flawfinder**: accuracy "almost on par"
- **Frama-C and IKOS**: "outstanding" (sound analysis tools)
- **CppCheck**: "high precision" (few FPs relative to findings)
- **Clang**: "fair recall" (decent detection)

#### Methodology
- Findings compared against Juliet manifest files (exact defect location by file+line)
- Match = true positive; report at other location = false positive
- Non-reported "bad" locations = false negatives; non-reported "good" = true negatives

### Other Comparative Findings
- Flawfinder detects maximum categories of vulnerabilities
- RATS and CppCheck similar in types of vulnerabilities detected
- CppCheck reports maximum false positives vs RATS and Flawfinder (contradicts "high precision" claim -- may be different studies)
- Sound tools (Frama-C, IKOS) are best but impractical for large codebases

### Note on Contradictions
- Some studies say Cppcheck has "high precision" (few FPs per finding)
- Other studies say Cppcheck has "most false positives" among compared tools
- Likely depends on: which CWEs tested, what version, and comparison group
