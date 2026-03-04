# Acceptable False Positive Rates - Industry Standards
**Source**: WebSearch
**Date fetched**: 2026-02-17
**Search query**: "acceptable false positive rate static analysis industry standard threshold typical"

## Content

### Search Results

1. [False Positives in Static Code Analysis (Parasoft)](https://www.parasoft.com/blog/false-positives-in-static-code-analysis/)
2. [Tackling False Negatives (Security Compass)](https://www.securitycompass.com/blog/safeguarding-software-quality-tackling-false-negatives-with-security-by-design/)
3. [Benchmark of False Positives with SAST Tools (Mend)](https://www.mend.io/blog/benchmark-of-false-positives/)
4. [Beating OWASP Benchmark (Qwiet AI)](https://qwiet.ai/beating-the-owasp-benchmark/)
5. [Reducing FPs with LLMs in Industry (arxiv)](https://arxiv.org/html/2601.18844v1)
6. [DeepSource <5% FP rate](https://deepsource.com/blog/how-deepsource-ensures-less-false-positives/)
7. [FPs Over Time (Chou/Coverity)](https://www.cs.umd.edu/~pugh/BugWorkshop05/papers/34-chou.pdf)
8. [SAST Evaluation Benchmark (Medium)](https://insbug.medium.com/static-source-code-security-scanning-tools-evaluation-benchmark-26764298f463)
9. [SAST FP Rate Comparison (Mobb)](https://www.mobb.ai/blog/sast-tools-false-positive-comparison)
10. [Reducing SAST FPs (Mend)](https://www.mend.io/blog/sast-false-positives/)

### KEY QUANTITATIVE DATA

#### Industry Standards/Targets
- **10-20% FP rate**: "Optimally acceptable" for SAST in development environments
- **5% FP rate**: Stringent goal (DeepSource, validated by major tech companies)
- **Google's guidance (2018)**: Low FP rates critical for developer adoption

#### Observed FP Rates in Practice
- **3% to 48%**: Range across 10 SAST tools (2018 study)
- **>95% FP rate**: State-of-the-art open-source SAST on Linux kernel (null pointer deref)
- **Highly variable**: Depends on project, rule set, and analysis depth

#### Context Matters
- Synthetic benchmarks (Juliet) ≠ production FP rates
- Production FP rates typically lower due to tuning
- Rule selection and configuration heavily impact FP rates

### Implications for SqC
- SqC at 56.2% FP rate is ABOVE the 10-20% "acceptable" threshold
- BUT this is on Juliet (synthetic), not production code
- Linux kernel null deref: >95% FP (worse than SqC)
- 3-48% range from 2018 study spans wide range
- SqC needs to target <20% FP for production viability
- OR position as "comprehensive scanning" tool where FPs are expected and triaged
