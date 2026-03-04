# ISSTA 2022: Effectiveness of Static C Code Analyzers
**Source**: WebSearch
**Date fetched**: 2026-02-17
**Search query**: "Empirical Study on the Effectiveness of Static C Code Analyzers for Vulnerability Detection" results false positive detection rate Cppcheck CodeQL

## Content

### Paper Details
- **Title**: An Empirical Study on the Effectiveness of Static C Code Analyzers for Vulnerability Detection
- **Venue**: ISSTA 2022 (ACM SIGSOFT International Symposium on Software Testing and Analysis)
- **ACM**: https://dl.acm.org/doi/10.1145/3533767.3534380
- **Preprint**: https://mediatum.ub.tum.de/doc/1659728/1659728.pdf
- **Artifacts**: https://dl.acm.org/do/10.5281/zenodo.6515687/full/

### Key Quantitative Results

#### Detection Rates (Vulnerability Miss Rate)
- **State-of-the-art tools miss 47% to 80% of vulnerabilities** in real-world programs
- Best single tool detection: ~53% (CommSCA)
- CodeQL: ~29% detection (24pp below CommSCA)
- Flawfinder: ~40% detection (13pp below CommSCA)
- **Cppcheck, CodeChecker, and Infer had the FEWEST vulnerabilities detected** (worst performers)

#### Tool Ranking (best to worst for detection)
1. CommSCA (commercial)
2. Flawfinder
3. CodeQL
4. Cppcheck (poor)
5. CodeChecker (poor)
6. Infer (poor)

#### False Positive Assessment
- No direct FP measurement (no ground truth)
- Used "proportion of functions flagged" as proxy
- CommSCA flagged slightly fewer functions than CodeQL (likely fewer FPs)

#### Tool Combination
- Combining tools reduces false negative rate to 30-69%
- Cost: 15 percentage points more functions flagged (more FPs)

### Study Scale
- 5 open-source + 1 commercial static C code analyzer
- 27 software projects
- 1.15 million lines of code
- 192 known vulnerabilities

### Key Takeaway for SqC
- Even commercial tools (CommSCA) miss ~47% of vulnerabilities
- Cppcheck is explicitly listed as one of the WORST performers
- Tool combination helps but increases FP burden
- SqC's AST-only approach puts it in similar category to Flawfinder (pattern-based)
