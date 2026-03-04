# Facebook Infer Static Analysis - Benchmark Results
**Source**: WebSearch
**Date fetched**: 2026-02-17
**Search query**: "Facebook Infer static analysis C benchmark accuracy false positive rate Juliet evaluation"

## Content

### Search Results

1. [Scalable Static Analysis Using Facebook Infer (PDF)](https://excel.fit.vutbr.cz/submissions/2019/059/59.pdf)
2. [Goseva-Popstojanova 2015 (PDF)](https://community.wvu.edu/~kagoseva/Papers/IST-2015.pdf)
3. [Facebook Infer (GitHub)](https://github.com/facebook/infer)
4. [Scaling Static Analyses at Facebook (CACM)](https://cacm.acm.org/research/scaling-static-analyses-at-facebook/)
5. [Infer homepage](https://fbinfer.com/)
6. [FuzzSlice: Pruning FPs (arxiv)](https://arxiv.org/html/2402.01923v1)

### KEY QUANTITATIVE DATA

#### Infer on Juliet Test Suite
- **1,923 total unique static warnings**
- **864 false positives**
- **1,059 true positives**
- **FP Rate: ~45%** (864/1923)
- **TP Rate: ~55%** (1059/1923)

#### Infer at Facebook Production
- Facebook claims **<20% FP rate** in production deployment
- (Production is heavily tuned vs raw benchmark)

#### L2D2 (Infer deadlock detector)
- 100% detection rate
- 11% FP rate
- On concurrent program benchmark (specialized)

### Comparison with SqC
| Metric | Infer (Juliet) | SqC (Juliet) |
|--------|---------------|--------------|
| FP Rate | ~45% | 56.2% |
| TP Rate | ~55% | 43.8% |
| Analysis Type | Bi-abduction, separation logic | AST-only |
| Price | Free | ? |
| CERT C Rules | Not CERT-focused | 283 rules |

### Key Takeaway
- **Infer's 45% FP rate on Juliet is VERY close to SqC's 56.2%**
- Infer uses much more sophisticated analysis (separation logic, bi-abduction)
- Yet Infer's FP rate is only ~11pp better than SqC on Juliet
- This suggests AST-only analysis can be competitive with advanced techniques on synthetic benchmarks
- Infer in production (<20% FP) shows tuning matters more than raw benchmark numbers
