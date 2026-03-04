# 2025 AI Code Security Benchmark: Snyk vs Semgrep vs CodeQL
**Source**: WebSearch (sanj.dev)
**Date fetched**: 2026-02-17
**Search query**: site:sanj.dev "AI Code Security Benchmark"

## Content

### Results

| Tool | Accuracy | FP Rate |
|------|----------|---------|
| **CodeQL** | **88%** | **5%** |
| **Snyk Code** | **85%** | **8%** |
| **Semgrep** | **82%** | **12%** |

### Notes
- CodeQL: Highest accuracy, lowest FP rate
- Semgrep: Lowest accuracy, highest FP rate (of these 3)
- Snyk Code: Middle ground
- These are NOT Juliet benchmarks -- likely diverse real-world codebases
- CodeQL uses deep semantic analysis (not comparable to AST-only)
- All three are multilanguage tools (not C-specific)
- These numbers should be taken with caveat: benchmark methodology matters enormously
