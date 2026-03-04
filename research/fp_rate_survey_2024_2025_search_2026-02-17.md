# Static Analysis FP Rate Survey 2024-2025
**Source**: WebSearch
**Date fetched**: 2026-02-17
**Search query**: "static analysis false positive rate benchmark survey comparison 2024 2025 tools evaluation"

## Content

### Search Results

1. [SAST Tools FP Rate Comparison (Mobb)](https://www.mobb.ai/blog/sast-tools-false-positive-comparison)
2. [Reducing FPs with LLMs (arxiv 2025)](https://arxiv.org/html/2601.18844v1)
3. [CASTLE: Benchmarking Dataset (PDF)](https://ssvlab.github.io/lucasccordeiro/papers/tase2025.pdf)
4. [CASTLE arxiv](https://arxiv.org/html/2503.09433v1)
5. [2025 AI Code Security Benchmark: Snyk vs Semgrep vs CodeQL](https://sanj.dev/post/ai-code-security-tools-comparison)
6. [Unified SAST benchmark (ScienceDirect)](https://www.sciencedirect.com/science/article/pii/S2352711025003024)
7. [Mitigating FP SA Warnings (IEEE TSE)](https://dl.acm.org/doi/10.1109/TSE.2023.3329667)

### KEY QUANTITATIVE DATA (2024-2025)

#### Tool FP Rates (AI/Modern SAST)
| Tool | FP Rate | Accuracy | Source |
|------|---------|----------|--------|
| **CodeQL** | **5%** | 88% | 2025 benchmark |
| **Snyk Code** | **8%** | 85% | 2025 benchmark |
| **Semgrep** | **12%** | 82% | 2025 benchmark |
| **Checkmarx** | **36.3%** | - | 2024 Tolly Report |
| One vendor | **16.7%** | - | Marketing claim |

#### Industry Context
- 2024 SOC survey: ~53% of all security alerts are FPs
- CASTLE Score: considers both TP and FP + severity; negative scores = FP triage burden too high
- Active research on LLM-based FP reduction (2025 papers)

#### Important New Resources
- **CASTLE dataset**: New benchmark for static code analyzers AND LLMs (2025)
  - https://arxiv.org/html/2503.09433v1
- **Unified SAST benchmark**: Compare AI-driven and traditional analyzers (2025)
  - https://www.sciencedirect.com/science/article/pii/S2352711025003024
- **2025 AI Code Security Benchmark**: Snyk vs Semgrep vs CodeQL
  - https://sanj.dev/post/ai-code-security-tools-comparison

### Implications for SqC
- Best-in-class tools (CodeQL, Snyk) achieve 5-12% FP rates in 2025
- BUT these are on different benchmarks (not Juliet CERT C)
- Checkmarx at 36.3% is more comparable (traditional SAST)
- SqC at 56.2% FP (Juliet) needs significant improvement for competitive positioning
- LLM-based FP filtering is an emerging approach (could complement SqC)
