# SAST Tools FP Rate Comparison (Mobb)
**Source**: WebSearch
**Date fetched**: 2026-02-17
**Search query**: site:mobb.ai SAST tools false positive comparison rate

## Content

### Key FP Rate Data Points

| Tool | FP Rate | Source |
|------|---------|--------|
| **Checkmarx** | **36.3%** | 2024 Tolly Report |
| **Fortify** | Not published | "inevitable" - uses rule tuning |
| **SonarQube** | **As low as 1%** | OWASP Benchmark specific testing |
| **Snyk** | Not stated | - |

### Context
- FP rates vary enormously depending on benchmark used
- SonarQube's 1% was on OWASP Benchmark Project (may not generalize)
- Checkmarx's 36.3% was from Tolly Report (independent testing)
- Fortify acknowledges FPs but relies on ML-based triage

### Important Context for SqC
- SqC at 56.2% FP rate on Juliet is higher than Checkmarx's 36.3% on Tolly
- BUT different benchmarks (Juliet vs Tolly/OWASP)
- Juliet is synthetic code; OWASP benchmark is also synthetic but different structure
- SqC's 283 CERT C rules likely covers more rules than Checkmarx
- These are web-focused SAST tools, not C-specific (except Fortify which does C)

### Follow-up
- Mobb also has: https://www.mobb.ai/blog/reduce-false-positives-tools
- https://www.mobb.ai/blog/ai-false-positive-fixing
