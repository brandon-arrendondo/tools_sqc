# SqC — Research

## Competitor Analysis (TODO)

### Tools to Evaluate

| Tool | Priority | Notes |
|------|----------|-------|
| Infer (Meta) | High | Flow-sensitive, separation logic |
| Frama-C (CEA) | High | Formal methods, Eva + WP plugins |
| Semgrep CE | Medium | Pattern-based baseline comparison |
| Flawfinder | Low | Lightweight CWE patterns |
| PVS-Studio | Low | Commercial, free for OSS |

### Key Metrics to Extract Per Tool

- TP Rate / FP Rate on Juliet or equivalent
- CWE coverage / CERT C rule coverage
- Analysis depth (AST / data-flow / inter-procedural / whole-program)
- Runtime performance
- Price / availability
- CI/CD integration (SARIF, GitHub, etc.)

### Academic Papers to Find

- ISSTA 2022 (TUM) — C analyzer comparison
- Goseva-Popstojanova & Perhinschi 2015 — Juliet evaluation
- JKU 2014 — Juliet scanner comparison
- NIST SATE IV/V/VI results

See `research/` directory for fetched content from prior research sessions.
