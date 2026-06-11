#!/usr/bin/env python3
"""
Full TP/FP adjudication of every sqc finding on libcrc (commit 7719e21),
scanned with conf/realworld/libcrc-rules.toml + the include/
and src/ context flags.

This is the real-world analog of Juliet's per-case oracle: every ENABLED-rule
finding is labelled. FP labels are the "should-stay-clean" oracle (Juliet's
good/OMITGOOD code); TP labels are genuine issues sqc should report (Juliet's
bad/OMITBAD code). Emits adjudication_libcrc_0.4.24.csv for
`python -m bench realworld-import-labels`.

Reproduce:  python3 data/precision_audit/libcrc/adjudicate.py
"""
import csv, json, os, sys

HERE = os.path.dirname(os.path.abspath(__file__))
SRC = os.path.join(HERE, "libcrc_cfg_0.4.24.json")
OUT = os.path.join(HERE, "adjudication_libcrc_0.4.24.csv")
PREFIX = "/home/brandon/toolchain/libcrc/"


def rel(f):
    return f.replace(PREFIX, "")


# Per-rule FP rationale for findings that are uniformly false alarms on libcrc.
FP_REASON = {
    "FIO47-C": "<inttypes.h> PRIxNN/PRIuNN macros in the concatenated format literal are not expanded by sqc's format parser; '%\"' is a macro-built specifier boundary, not an invalid one",
    "PRE32-C": "PRIxNN are object-like macros from <inttypes.h>; PRE32-C targets #if/#include directives passed as macro arguments, not standard format-string macros",
    "INT36-C": "'(uintNN_t) *ptr' dereferences ptr to a byte then widens; it is not a pointer-to-integer conversion (sqc confuses *ptr with ptr)",
    "API00-C": "input_str validated indirectly: 'ptr = input_str; if (ptr != NULL)' guards every deref (argv guarded by the argc check); sqc misses the alias",
    "MEM30-C": "target 'crc_tabXX' is a file-scope static array; init writes a scalar to a global element, no stack object escapes",
    "INT31-C": "value is bounded: ch in [0,255] after the !=EOF guard (fgetc contract) so '(unsigned char)ch' is lossless; nibble<<4 fits unsigned char",
    "INT32-C": "counter bounded far below INT_MAX (test-failure count / argv index / byte value 0-255); no feasible overflow",
    "INT30-C": "unsigned-shift wrap is well-defined and intended in the CRC algorithm; result is masked",
    "INT16-C": "the '&' operand is the unsigned uint64_t element crc_tab_precalc[a]; 'a' is only the index, not the masked value",
    "INT14-C": "'crc' is unsigned (uint16_t/uint64_t), not signed; the shift is part of the CRC algorithm",
    "INT17-C": "0xffffffffL fits both 32- and 64-bit long and is masked into a uint32_t; no portability defect",
    "INT01-C": "'filename' is a path string, not a size/count; size_t is inapplicable",
    "ARR00-C": "loop variable 'a' is initialized in the for-init (a=0); not uninitialized",
    "ARR02-C": "standard C idiom ('char *argv[]' / 'extern T x[]'); the array size is defined elsewhere",
    "API02-C": "main(int, char*[]) is the standard entry-point signature; a size parameter is inapplicable",
    "ENV01-C": "no getenv()/environment-variable use in the flagged code",
    "STR31-C": "argc is validated before argv use (argc<2 / argc!=3 -> exit)",
    "STR03-C": "snprintf(result, 3, ...) writes exactly 2 hex + NUL into the documented >=3-byte buffer; bounded by design",
    "FIO34-C": "'ch' is int; '(ch=fgetc())!=EOF' is the correct EOF-detection idiom",
    "FIO23-C": "stdout is flushed at normal program exit; no data loss",
    "FIO02-C": "demo CLI: the argv path is the intended user interface; no privilege boundary to canonicalize across",
    "EXP14-C": "implicit promotion is benign and intended in the CRC bit math; values fit int and are masked back",
    "EXP19-C": "missing braces around a single-statement body; cosmetic, no defect",
    "EXP20-C": "'if(!strcmp(...))' is a standard, well-understood idiom",
    "DCL06-C": "256/0xFF/0x0001 are intrinsic CRC constants; a symbolic name adds no safety",
    "DCL16-C": "lowercase suffix follows hex digits/'u' (e.g. 0x..ull); negligible 'l'/'1' confusion risk",
    "DCL04-C": "two related ints in one declaration; cosmetic",
    "ERR33-C": "fgetc result is checked against EOF; an fclose failure on a read-only stream is inconsequential",
}


def adjudicate(v):
    rid, m = v["rule_id"], v["message"]
    f = rel(v["file"])
    # ---- genuine issues sqc should report (TP / OMITBAD analog) ----
    if rid == "FIO40-C":
        return "TP", "input_string is used in the do_ascii/do_hex paths after fgets() failure (lines 102-105 only perror, no reset) -> indeterminate read"
    if rid == "FIO20-C":
        return "TP", "fgets(MAX-1) truncation is never detected; long input is silently truncated"
    if rid == "DCL00-C":
        return "TP", "sht75_crc_table is initialized once and never modified; const-qualifying it is correct and implementable"
    if rid == "PRE06-C":
        return "TP", "header genuinely lacks an include guard (no #ifndef/#define)"
    if rid == "DCL15-C":
        return "TP", "update_crc_64 has external linkage but is not declared in checksum.h nor used cross-file; should be static or given a prototype"
    if rid == "EXP33-C":
        if "input_string" in m:
            return "TP", "input_string may be read uninitialized after fgets() failure (same defect as FIO40-C)"
        return "FP", "CRC table is lazily initialized via 'if(!_init) init_tab()' before any read; fully populated when read"
    if rid == "CON03-C":
        if "_init" in m:
            return "TP", "lazy table init (init flag + table write) is an unsynchronized data race if crc_* is called from multiple threads (known libcrc thread-safety issue)"
        return "FP", "single-threaded test counter / read-only const table; no concurrent access"
    if rid in ("EXP12-C", "ERR00-C") and f == "precalc/precalc.c" and "fclose" in m:
        return "TP", "fclose() on a written stream is unchecked; a close/flush failure (e.g. disk full) silently loses the generated table"
    # ---- everything else: false alarm (FP / OMITGOOD analog) ----
    if rid == "EXP12-C":
        return "FP", "ignoring the printf/fprintf/snprintf return value is idiomatic here; no error-handling consequence"
    if rid == "ERR00-C":
        return "FP", "fopen/fgetc return IS checked (if(fp..) / !=EOF); fclose on a read stream is benign"
    if rid == "ARR30-C":
        if "size 256" in m or "0x00FF" in m or "0x00ff" in m:
            return "FP", "table index is masked '& 0x00FF' to 0-255 against a 256-entry table; provably in-bounds"
        return "FP", "NUL/sentinel-terminated string scan; the loop terminates, no buffer overflow"
    return "FP", FP_REASON.get(rid, "advisory finding with no security or correctness merit on this code")


def main():
    data = json.load(open(SRC))
    rows = []
    for i, v in enumerate(sorted(data, key=lambda x: (x["rule_id"], rel(x["file"]), x["line"]))):
        verdict, reason = adjudicate(v)
        rows.append({
            "rule": v["rule_id"], "idx": i, "project": "libcrc",
            "file": rel(v["file"]), "line": v["line"],
            "verdict": verdict, "reason": reason,
        })
    with open(OUT, "w", newline="") as fh:
        w = csv.DictWriter(fh, fieldnames=["rule", "idx", "project", "file", "line", "verdict", "reason"])
        w.writeheader()
        w.writerows(rows)
    tp = sum(r["verdict"] == "TP" for r in rows)
    fp = sum(r["verdict"] == "FP" for r in rows)
    print(f"wrote {OUT}: {len(rows)} findings  ({tp} TP, {fp} FP, precision {tp/len(rows)*100:.1f}%)")


if __name__ == "__main__":
    main()
