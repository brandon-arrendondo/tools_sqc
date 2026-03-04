# Cppcheck Juliet Benchmark Search
**Source**: WebSearch
**Date fetched**: 2026-02-17
**Search query**: "cppcheck Juliet benchmark false positive rate accuracy detection rate evaluation"

## Content

### Search Results

1. [Benchmarking Static Analysis Tools for C | CodeX (Medium)](https://medium.com/codex/11-static-analysis-tools-for-c-4fe5f63c18a5) -- likely has concrete numbers
2. [LLM-Driven FP reduction (arxiv 2511.04023)](https://arxiv.org/pdf/2511.04023)
3. [Test Suites as Training Data (arxiv 2105.03523)](https://arxiv.org/pdf/2105.03523)
4. [Juliet Dynamic Test Suite (GitHub)](https://github.com/ispras/juliet-dynamic)
5. [CAST Juliet and OWASP Benchmark Results](https://www.castsoftware.com/pulse/juliet-and-owasp-benchmark-results-how-cast-tests)
6. [TUM Paper (PDF)](https://mediatum.ub.tum.de/doc/1659728/1659728.pdf)

### Key Findings

#### General Juliet Benchmark Performance (from research)
- Some tools found nearly nothing; others found >50% of tested vulnerabilities
- Some tools had ~50% precision (every second finding was a TP)
- Some tools required reviewing hundreds of findings to find one TP
- **Tools on average found about 20% of weaknesses in basic test cases**
- Complex control flow/data flow constructs significantly reduced success rates

#### Cppcheck Specific
- One study: detection rate around 83.5% (but this seems high -- likely on specific CWE subset)
- Cppcheck philosophy: minimize false positives at cost of false negatives
- In ISSTA 2022 study, Cppcheck was one of the WORST performers for vulnerability detection

### Key Data Points for SqC Comparison
- **Average tool finds ~20% of weaknesses in basic Juliet test cases**
- SqC at 43.8% TP rate appears to be ABOVE average for Juliet benchmarks
- Complex patterns reduce detection significantly (SqC is AST-only, so similar limitation)
