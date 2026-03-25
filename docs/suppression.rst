Suppression System
==================

SqC supports suppressing false positives via inline source comments or an external
TOML file. Each suppression includes a SHA-256 hash of the violation line, ensuring
suppressions break automatically when the underlying code changes.

Inline Comment Suppression
--------------------------

.. code-block:: c

    // Line-before style (most common):
    // SQC-SUPPRESS: ARR30-C HASH:a1b2c3d4e5f67890 JUSTIFICATION: "Bounds validated by caller"
    arr[index] = value;

    // Inline style (same line as violation):
    arr[index] = value; // SQC-SUPPRESS: ARR30-C HASH:a1b2c3d4e5f67890 JUSTIFICATION: "Bounds checked"

    // Stacked (multiple rules on one line):
    // SQC-SUPPRESS: ERR00-C HASH:aaaa... JUSTIFICATION: "return captured in bytes_read"
    // SQC-SUPPRESS: EXP34-C HASH:bbbb... JUSTIFICATION: "buf checked at function entry"
    bytes_read = fread(buf, 1, file_size, fp);

Generate the hash with:

::

    sqc --generate-suppression src/main.c:42:ARR30-C

External Suppression File
-------------------------

For read-only codebases, place a ``.sqc-suppress.toml`` in the project root
(auto-detected) or specify with ``--suppress-file``:

.. code-block:: toml

    # .sqc-suppress.toml

    [[suppression]]
    file = "ringbuffer.c"
    rule = "INT30-C"
    hash = "a1f5861150a1e5b8"
    justification = "Overflow checked by caller"

    [[suppression]]
    file = "src/utility.c"
    rule = "EXP34-C"
    hash = "b2c3d4e5f6a78901"
    justification = "Pointer validated at function entry"

The ``file`` field matches by suffix -- ``ringbuffer.c`` matches any path ending
in ``ringbuffer.c``.

Hash Details
------------

- **Algorithm**: ``SHA-256(rule_id + ":" + whitespace_normalized(violation_line))``,
  truncated to 16 hex characters
- **Rule-scoped**: different rules on the same line produce different hashes
- **Proximity matching**: inline comments match within 5 lines before the violation
- **Staleness detection**: if the violation line changes, the hash no longer matches
  and the suppression stops working, forcing re-review
