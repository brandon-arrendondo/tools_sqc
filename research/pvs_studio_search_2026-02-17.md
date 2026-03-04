# PVS-Studio Juliet CWE Detection Benchmark
**Source**: WebSearch
**Date fetched**: 2026-02-17
**Search query**: "PVS-Studio Juliet CWE detection benchmark comparison false positive rate"

## Content

### Search Results

1. [PVS-Studio CWE Classification](https://pvs-studio.com/en/pvs-studio/sast/cwe/)
2. [Juliet Dynamic Test Suite (GitHub)](https://github.com/ispras/juliet-dynamic)
3. [LLM FP reduction (arxiv)](https://arxiv.org/pdf/2511.04023)
4. [CAST Juliet and OWASP Benchmark Results](https://www.castsoftware.com/pulse/juliet-and-owasp-benchmark-results-how-cast-tests)
5. [CASTLE: Benchmarking Dataset (PDF)](https://ssvlab.github.io/lucasccordeiro/papers/tase2025.pdf)

### Key Findings

#### PVS-Studio
- Has CWE classification page: maps its diagnostics to CWE IDs
- **No public Juliet benchmark results found**
- PVS-Studio does not appear to publish Juliet FP/TP rates
- Commercial tool with free tier for open source

#### Juliet Test Suite v1.3
- 64,295 test cases covering 118 CWEs
- Ground-truth vulnerability labels for precise precision/recall measurement
- Comes with comparison script (TPR and FPR metrics)

#### General Finding
- "Static analyzers suffer from high false positives"
- Some approaches reduce FPs by 43.7% vs state-of-the-art baselines

### PVS-Studio Overview
- **Analysis Type**: Data-flow analysis, pattern matching, symbolic execution
- **Price**: Commercial (~$2K-15K/year depending on team size)
- **CWE Coverage**: Maps diagnostics to CWEs (see pvs-studio.com/en/pvs-studio/sast/cwe/)
- **CERT C**: Partial support (maps some diagnostics)
- **Juliet Data**: Not publicly available
