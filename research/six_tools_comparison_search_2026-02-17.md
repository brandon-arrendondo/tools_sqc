# Critical Comparison of Six Static Analysis Tools (2022)
**Source**: WebSearch
**Date fetched**: 2026-02-17
**Search query**: "critical comparison" "six static analysis tools" detection agreement precision results

## Content

### Paper Details
- **Title**: A critical comparison on six static analysis tools: Detection, agreement, and precision
- **Authors**: Lenarduzzi, Pecorelli, Saarimaki, Lujan, Palomba
- **Published**: 2022
- **arxiv**: https://arxiv.org/abs/2101.08832
- **ScienceDirect**: https://www.sciencedirect.com/science/article/pii/S0164121222002515
- **PDF**: https://fpalomba.github.io/pdf/Journals/J51.pdf

### Tools Compared (Java)
1. Better Code Hub
2. CheckStyle
3. Coverity Scan
4. FindBugs
5. PMD
6. SonarQube

### Key Results

#### Precision
- **FindBugs: 57% precision** (43% of rules were FPs)
- Other tools: not specifically stated in search results but "low degree of precision" overall

#### Inter-Tool Agreement
- **"Little to no agreement" among the tools**
- Tools identify different issues at line- and class-level
- Manually analyzed agreement

#### Study Scale
- 47 Java projects
- 6 tools applied to each

### Key Takeaway
- Even well-known tools have ~40%+ FP rates (FindBugs at 43% FP)
- Tools don't agree with each other on what's a bug
- This validates that FP rates in the 40-60% range are NORMAL for SA tools
- SqC's 56.2% FP rate is comparable to FindBugs' 43% FP rate
- Note: This study is Java-focused, not C -- but principles apply

### NOTE
- This study is about code quality tools, not security/CWE tools
- Security-focused tools may have different FP characteristics
