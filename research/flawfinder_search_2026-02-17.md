# Flawfinder Static Analysis - Benchmark Results
**Source**: WebSearch
**Date fetched**: 2026-02-17
**Search query**: "flawfinder static analysis benchmark accuracy CWE false positive rate Juliet evaluation"

## Content

### Search Results

1. [Benchmarking Static Analysis Tools for C (Medium)](https://medium.com/codex/11-static-analysis-tools-for-c-4fe5f63c18a5)
2. [Goseva-Popstojanova 2015 (PDF)](https://community.wvu.edu/~kagoseva/Papers/IST-2015.pdf)
3. [Flawfinder GitHub](https://github.com/david-a-wheeler/flawfinder)
4. [TUM ISSTA 2022 (PDF)](https://mediatum.ub.tum.de/doc/1659728/1659728.pdf)
5. [Comparative Study of SAST tools (ResearchGate)](https://www.researchgate.net/publication/341903726)
6. [CASTLE Benchmark (PDF)](https://ssvlab.github.io/lucasccordeiro/papers/tase2025.pdf)
7. [Flawfinder homepage](https://dwheeler.com/flawfinder/)

### Key Findings

#### Flawfinder Analysis Approach
- **Lexical scanning** (token-based, even simpler than AST)
- No control flow, data flow, or data type information
- CWE-Compatible
- "Will necessarily produce many false positives"

#### Juliet Performance
- Accuracy "almost on par" with AdLint, Clang-Tidy, CppCheck
- Detected "maximum categories of vulnerabilities" (broadest coverage)
- Among lexical tools (ITS4, Flawfinder, RATS): similar TP detection rate
- ITS4 better at filtering FPs than Flawfinder

#### ISSTA 2022 Ranking (real-world vulnerabilities)
- Flawfinder ranked #2 behind CommSCA (commercial)
- CommSCA outperformed Flawfinder by 13 percentage points
- Flawfinder outperformed CodeQL (which is much more sophisticated)

### Comparison with SqC
| Feature | Flawfinder | SqC |
|---------|-----------|-----|
| Analysis | Lexical scanning | AST (tree-sitter) |
| Depth | Token-level only | Full AST structure |
| FP Rate | "Many" (expected high) | 56.2% |
| CWE Coverage | CWE-Compatible | CERT C (283 rules) |
| Price | Free | ? |
| Speed | Very fast | Fast |

### Key Takeaway
- Flawfinder is SIMPLER than SqC (lexical vs AST) yet performs comparably or better on some benchmarks
- This validates that lightweight approaches can be effective
- Flawfinder's "many FPs" are accepted because it's free and fast
- SqC's AST-level analysis should theoretically allow better FP filtering than Flawfinder
