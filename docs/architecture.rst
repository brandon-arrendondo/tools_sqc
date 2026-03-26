Analysis Architecture
=====================

SqC uses a multi-pass analysis architecture:

::

    Source Files
        |
        v
    [Tree-sitter Parser] --> AST (per-file)
        |
        v
    [Pre-scan Pass] --> Cross-file context (function defs, summaries, macros)
        |
        v
    [CFG Construction] --> Per-function control-flow graphs
        |
        v
    [Dataflow Analysis] --> Reaching definitions, null state lattice
        |
        v
    [Rule Evaluation] --> 283 CERT C rules applied to AST + CFG + context
        |
        v
    [Suppression Filter] --> Hash-based + wildcard (glob/prefix) suppression
        |
        v
    [Export] --> CSV, XLSX, JSON, SARIF

Key capabilities:

- **Tree-sitter parsing**: Fast, incremental, error-tolerant C parsing
- **Cross-file pre-scan**: Function definitions, summaries, and macro aliases
  collected across directories before per-file analysis
- **Control-flow graphs**: Per-function CFG with ``condition_range`` metadata
  for path-sensitive analysis
- **Null state dataflow**: Forward lattice (Unknown / DefinitelyNull / PossiblyNull
  / NotNull) with edge refinement on branch conditions
- **Inter-procedural summaries**: Null return behavior, freed parameters,
  no-return functions propagated across call sites
- **Standard function database**: ~370 C11, POSIX, and Windows API functions
  recognized to avoid false positives on standard library calls

Current Capabilities (v0.2.7+)
------------------------------

====================================  =====================================================
Capability                            Implementation
====================================  =====================================================
Local variable/type inference         Per-function ``collect_variable_types``
Preprocessor block traversal          ``preproc_*`` node recursion
Standard function database            ~370 C11/POSIX/Windows functions
Cross-file function scanning          ``-d`` flag pre-scan
CFG construction                      Per-function with ``condition_range`` metadata
Reaching definitions                  Data-flow for path-sensitive analysis
Inter-procedural summaries            Null returns, freed params, no-return
CFG-based null state dataflow         Forward dataflow with NullState lattice
Taint tracking                        FIO30-C
Variable state tracking               EXP33-C uninitialized detection
====================================  =====================================================

Known Limitations
-----------------

==============================  ==============================================
Gap                             Impact
==============================  ==============================================
No preprocessor expansion       Macros appear as function calls
No alias analysis               Pointer aliasing unresolved
No symbolic execution           Can't evaluate complex expressions
No SSA form                     No use-def chains beyond reaching defs
No value range analysis         Beyond literal constants
No whole-program analysis       Limited to function summary pre-scan
==============================  ==============================================

Architectural Ceiling
---------------------

The ~48% Juliet TP rate is likely near the ceiling for single-TU AST analysis.
Without value-range and alias analysis, the tool cannot distinguish validated
from unvalidated inputs, null-checked from unchecked pointers, or computed from
literal buffer sizes.

Competitor Landscape
--------------------

==============  ==============  =========  ====================================  ===========
Tool            Detection Rate  FP Rate    Analysis Depth                        Price
==============  ==============  =========  ====================================  ===========
**SqC**         48.4%           51.6%      AST + CFG + inter-procedural          --
Semgrep CE      44--48%         Very low   AST (tree-sitter)                     Free
Semgrep Pro     72--75%         Very low   AST + taint + inter-file              Commercial
Infer           ~55%            ~45%       Separation logic                      Free
Flawfinder      ~40%            High       Lexical scanning                      Free
Cppcheck        Low             Very low   Data-flow                             Free
Coverity        Best-in-class   ~15--20%   Inter-procedural, path-sensitive      Enterprise
==============  ==============  =========  ====================================  ===========

**Key context**: Tools on average find ~20% of weaknesses in Juliet
(ISSTA2022). Even commercial tools miss 27% (Goseva2015). Industry FP target
for adoption is 10--20%. See :doc:`bibliography` for full references.
