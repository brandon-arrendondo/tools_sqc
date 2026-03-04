# Polyspace CERT C Compliance Coverage
**Source**: WebSearch
**Date fetched**: 2026-02-17
**Search query**: "Polyspace CERT C compliance coverage number of rules static analysis benchmark"

## Content

### Search Results

1. [Addressing Security - CERT C Conformity with Polyspace (MathWorks)](https://www.mathworks.com/products/polyspace/static-analysis-notes/cert-c.html)
2. [Polyspace (Sciengineer)](https://sciengineer.com/products/polyspace/)
3. [What Is CERT C? (MathWorks)](https://www.mathworks.com/discovery/cert-c.html)
4. [Polyspace (Wikipedia)](https://en.wikipedia.org/wiki/Polyspace)
5. [Polyspace Support for Coding Standards (MathWorks)](https://www.mathworks.com/help/bugfinder/ug/polyspace_coverage_coding_standard.html)
6. [Polyspace Bug Finder (MathWorks)](https://www.mathworks.com/products/polyspace-bug-finder.html)

### Key Findings

#### CERT C Coverage
- **Polyspace Bug Finder: FULL compliance checking for ALL CERT C rules** (since R2020a)
- Also checks some CERT C recommendations (not just rules)
- Uses "deep semantic analysis" capabilities
- Supports ALL statically enforceable rules in CERT C standard

#### Analysis Capabilities
- Abstract interpretation-based analysis (Polyspace Code Prover)
- Pattern-based checking (Polyspace Bug Finder)
- Can prove absence of certain runtime errors (sound analysis)

#### Products
- **Polyspace Bug Finder**: Pattern-based, fast, finds defects
- **Polyspace Code Prover**: Formal verification, proves absence of errors

### Comparison with SqC
| Feature | Polyspace Bug Finder | Polyspace Code Prover | SqC |
|---------|---------------------|----------------------|-----|
| CERT C Rules | All statically enforceable | All provable | 283 |
| Analysis Type | Deep semantic | Abstract interpretation | AST-only |
| FP Rate | Not published | Near-zero (sound) | 56.2% |
| Price | $$$ (MathWorks license) | $$$ | ? |
| Target Market | Embedded/automotive/aerospace | Safety-critical | General C |

### Key Takeaway
- Polyspace is the gold standard for CERT C coverage (all rules since R2020a)
- But it's expensive enterprise software (MathWorks ecosystem)
- SqC's 283 rules is close to but possibly less than Polyspace's full coverage
- Polyspace has much deeper analysis (semantic vs AST-only)
