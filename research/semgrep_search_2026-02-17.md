# Semgrep C Static Analysis - CERT Coverage & Benchmarks
**Source**: WebSearch
**Date fetched**: 2026-02-17
**Search query**: "Semgrep C static analysis CERT rules coverage false positive rate benchmark"

## Content

### Search Results

1. [Comparing Semgrep CE and Semgrep Code (2025)](https://semgrep.dev/blog/2025/security-research-comparing-semgrep-community-edition-and-semgrep-code-for-static-analysis/)
2. [Benchmarking Semgrep CE Performance (2025)](https://semgrep.dev/blog/2025/benchmarking-semgrep-performance-improvements/)
3. [Semgrep GitHub](https://github.com/semgrep/semgrep)
4. [Zero FP SAST (2025)](https://semgrep.dev/blog/2025/making-zero-false-positive-sast-a-reality-with-ai-powered-memory/)
5. [C/C++ Static Analysis (2024)](https://semgrep.dev/blog/2024/modernizing-static-analysis-for-c/)

### Key Findings

#### Detection Rates (Semgrep on benchmark projects)
- **Semgrep Community Edition (CE)**: 44-48% of true positives detected
- **Semgrep Code (Pro)**: 72-75% of true positives detected (50-71% improvement over CE)
- **FP Rate**: Very low - 0 to 1 FP per project in benchmark testing

#### C/C++ Support
- Uses tree-sitter grammars (SAME as SqC!)
- Focus on practical programming patterns
- Claims coverage as comprehensive as traditional SAST
- C/C++ support announced in 2024 blog post

#### CERT C Coverage
- **No specific CERT C rules mentioned** in search results
- Semgrep is rule/pattern-based -- community can write CERT rules
- Semgrep Registry may have some CERT C rules (need to check)

#### Architecture
- Pattern matching based on AST (tree-sitter)
- Pro version adds inter-file analysis, taint tracking, constant propagation
- Community Edition is AST-only (similar to SqC)

### Comparison with SqC
| Feature | Semgrep CE | Semgrep Pro | SqC |
|---------|-----------|-------------|-----|
| TP Rate | 44-48% | 72-75% | 43.8% |
| FP Rate | Very low | Very low | 56.2% |
| Analysis | AST (tree-sitter) | AST + taint + inter-file | AST (tree-sitter) |
| CERT C | Community rules | Community rules | 283 built-in |
| Price | Free | Commercial | ? |

### KEY INSIGHT
- **Semgrep CE achieves 44-48% TP with very low FP** -- similar TP to SqC but MUCH better FP
- Both use tree-sitter AST -- Semgrep's pattern approach may be more selective
- SqC's advantage: 283 built-in CERT C rules vs Semgrep's community-maintained rules
- Semgrep Pro's 72-75% TP shows what inter-file + taint tracking adds
