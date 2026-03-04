# Fortify Static Analysis - SATE/Benchmark Results
**Source**: WebSearch
**Date fetched**: 2026-02-17
**Search query**: "Fortify static analysis SATE results accuracy false positive rate CERT benchmark"

## Content

### Search Results

1. [SAST with Fortify (Medium)](https://medium.com/globant/static-application-security-testing-sast-with-fortify-93ef52a03f21)
2. [Fortify SCA | OpenText](https://www.microfocus.com/en-us/cyberres/application-security/static-code-analyzer)
3. [Benchmarking Web App SAST (ResearchGate)](https://www.researchgate.net/publication/342597384)
4. [Beyond the Noise: Fortify Precision (OpenText blog)](https://community.opentext.com/cyberres/b/cybersecurity-blog/posts/beyond-the-noise-elevating-sast-with-fortify-s-precision-and-innovation)
5. [**SAST Tools Compared by FP Rate (Mobb)**](https://www.mobb.ai/blog/sast-tools-false-positive-comparison) -- KEY RESOURCE
6. [Fortify SCA Data Sheet (PDF)](https://www.microfocus.com/en-gb/media/data-sheet/fortify-static-code-analyzer-static-application-security-testing-ds-a4.pdf)
7. [Fortify reviews (PeerSpot)](https://www.peerspot.com/products/fortify-static-code-analyzer-reviews)
8. [Fortify Gartner Reviews](https://www.gartner.com/reviews/market/application-security-testing/vendor/opentext/product/fortify-static-code-analyzer)
9. [AI-driven static analysis auditing (OpenText)](https://blogs.opentext.com/increase-speed-and-accuracy-with-ai-driven-static-analysis-auditing/)

### Key Findings

#### Fortify Performance
- **OWASP 1.2b Benchmark: 100% true positive rate** (claimed)
- **815 unique vulnerability categories** covered
- **1M+ individual APIs** covered
- False positives acknowledged as "inevitable"
- ML-based Audit Assistant classifies issues with "up to 98% accuracy"
- Does NOT publish specific Juliet benchmark results

#### Fortify FP Management
- Rule tuning and prioritization features
- Audit Assistant 2.0: trained on hundreds of millions of anonymized audit decisions
- "Anonymized issue metrics" sent to scan analytics for ML training

### IMPORTANT: Mobb FP Rate Comparison
- **https://www.mobb.ai/blog/sast-tools-false-positive-comparison** -- likely has direct FP rate comparison across SAST tools
- MUST follow up on this resource

### Fortify Overview
- **Analysis Type**: Inter-procedural, data-flow, control-flow, taint analysis
- **Price**: Enterprise commercial (OpenText/Micro Focus) - $$$
- **CERT C**: Supports some CERT C rules
- **Languages**: 25+ languages
- **Juliet/SATE Data**: 100% TPR on OWASP benchmark (marketing claim), no public Juliet data
