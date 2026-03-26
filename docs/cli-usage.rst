Advanced CLI Usage
==================

Full Command Reference
----------------------

::

    sqc [OPTIONS] [PATH]

    Arguments:
      [PATH]  Path to the file, directory, or git repository to analyze [default: .]

    Options:
      -m, --manifest <FILE>            Path to the rules manifest file
                                       [default: rules_templates/rules-all.toml]
      -i, --interactive                Run in interactive terminal UI mode
      -e, --export <FILE>              Export violations to file (format by extension:
                                       .csv, .xlsx, .json, .sarif, .sarif.json)
          --generate-suppression <FILE:LINE:RULE>
                                       Generate suppression entry for a specific violation
      -d, --directories <DIR>          Additional directories to pre-scan for function
                                       definitions (repeatable; enables cross-file context)
          --fail-on-violation          Exit with code 1 if any violations are found
          --fail-on-severity <LEVEL>   Exit with code 1 if any violation meets or exceeds
                                       this severity [Low, Medium, High, Critical]
          --min-severity <LEVEL>       Only report violations at or above this severity
                                       [Low, Medium, High, Critical]
          --rules <RULE1,RULE2,...>    Only report violations from these rules (comma-separated)
          --diff                       Only analyze modified/new C files (requires git repo)
          --suppress-file <FILE>       Path to .sqc-suppress.toml file
                                       (auto-detected in project root if not specified;
                                       supports [[suppression]] hash entries and
                                       [[wildcard]] glob/prefix entries)
      -I, --include <DIR>              Additional include directories (for header resolution)
      -h, --help                       Print help
      -V, --version                    Print version


Cross-File Analysis
-------------------

The ``-d`` / ``--directories`` flag enables cross-file context by pre-scanning
directories for function definitions, type declarations, and macro aliases. This
significantly reduces false positives from rules like DCL31-C (unused identifiers)
and DCL07-C (type mismatches) that would otherwise flag externally-defined symbols.

::

    # Pre-scan the project directory for cross-file context
    sqc /path/to/project -d /path/to/project

    # Include additional directories (e.g., shared headers, sibling modules)
    sqc /path/to/project -d /path/to/project -d /path/to/shared/headers

    # Multiple -d flags stack — all are pre-scanned before analysis begins
    sqc src/ -d src/ -d vendor/ -d third_party/

The pre-scan collects:

- **Function definitions**: names, parameter counts, return types
- **Function summaries**: null return behavior, freed parameters, no-return annotations
- **Macro aliases**: ``#define SYSTEM system`` patterns resolved for taint tracking
- **Local variable states**: assignments within function bodies for call-site analysis


Export Formats
--------------

SqC determines the export format from the file extension:

=========== ===============================================================
Extension   Format
=========== ===============================================================
``.csv``    Comma-separated values (file, line, column, rule, severity, message)
``.xlsx``   Excel workbook with formatted columns and severity coloring
``.json``   JSON array of violation objects
``.sarif``  `SARIF 2.1.0 <https://sarifweb.azurewebsites.net/>`_ for IDE and CI integration
=========== ===============================================================

::

    sqc /path/to/repo --export results.csv
    sqc /path/to/repo --export results.xlsx
    sqc /path/to/repo --export results.json
    sqc /path/to/repo --export results.sarif

JSON export produces an array of violation objects, each containing:

.. code-block:: json

    {
        "file": "src/main.c",
        "line": 42,
        "column": 5,
        "rule_id": "ARR30-C",
        "severity": "High",
        "message": "Do not form or use out-of-bounds pointers or array subscripts",
        "suggestion": "Validate array index before use"
    }


Severity Filtering
------------------

Control which violations are reported and which trigger failure:

::

    # Only report Medium and above (suppress Low-severity noise)
    sqc /path/to/repo --min-severity Medium

    # Fail only on High or Critical (gate CI but still report Medium)
    sqc /path/to/repo --min-severity Medium --fail-on-severity High

    # Strict mode: fail on any violation
    sqc /path/to/repo --fail-on-violation


Rule Filtering
--------------

Restrict analysis to specific rules:

::

    # Only check memory and array rules
    sqc /path/to/repo --rules MEM30-C,MEM31-C,ARR30-C,ARR32-C

    # Combine with severity and export
    sqc /path/to/repo --rules STR31-C,STR32-C --min-severity High --export str-results.sarif


Diff Mode
---------

Analyze only files modified in the current git working tree (staged + unstaged
changes vs HEAD):

::

    # Only analyze changed C files
    sqc /path/to/repo --diff

This is particularly useful in CI pipelines to provide fast feedback on pull
requests without scanning the entire codebase.


Exit Codes
----------

======  ==========================================================================
Code    Meaning
======  ==========================================================================
``0``   Success (no violations, or none meeting the failure threshold)
``1``   Violations found (when ``--fail-on-violation`` or ``--fail-on-severity`` is set)
``2``   Analysis error (invalid path, bad manifest, parse failure)
======  ==========================================================================
