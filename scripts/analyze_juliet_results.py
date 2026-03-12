#!/usr/bin/env python3
"""
Analyze SqC results against Juliet ground truth to calculate:
- True Positive Rate (TPR)
- False Positive Rate (FPR)
- False Negative Rate (FNR)

Optional CWE-aware metrics (with --cwe and --rule-cwe-map):
- CWE-matched TP/FP rates (filtered to relevant rules only)
- Noise ratio (non-CWE-matched findings)
- Per-file detection rate
- FLAW-line hit rate with ±1 tolerance
"""

import csv
import json
import re
import os
from pathlib import Path
from collections import defaultdict

def parse_c_file_sections(filepath):
    """
    Parse a Juliet C file to identify OMITBAD and OMITGOOD line ranges.
    Returns: {
        'bad_lines': set of line numbers in OMITBAD sections,
        'good_lines': set of line numbers in OMITGOOD sections,
        'flaw_lines': set of line numbers with FLAW comments
    }
    """
    result = {
        'bad_lines': set(),
        'good_lines': set(),
        'flaw_lines': set()
    }

    with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()

    in_bad = False
    in_good = False

    for i, line in enumerate(lines, start=1):
        # Track FLAW comments
        if 'FLAW:' in line or 'POTENTIAL FLAW:' in line:
            result['flaw_lines'].add(i)

        # Track section boundaries
        if '#ifndef OMITBAD' in line:
            in_bad = True
            in_good = False
        elif '#endif /* OMITBAD */' in line or '#endif  /* OMITBAD */' in line:
            in_bad = False
        elif '#ifndef OMITGOOD' in line:
            in_good = True
            in_bad = False
        elif '#endif /* OMITGOOD */' in line or '#endif  /* OMITGOOD */' in line:
            in_good = False

        # Record line numbers
        if in_bad:
            result['bad_lines'].add(i)
        elif in_good:
            result['good_lines'].add(i)

    return result

def parse_sqc_csv(csv_path):
    """
    Parse SqC CSV output to get violations by file and line.
    Returns: {
        'filename.c': {
            line_num: [rule_id1, rule_id2, ...]
        }
    }
    """
    violations = defaultdict(lambda: defaultdict(list))

    with open(csv_path, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            title = row['Title']
            # Parse format: "RULE-ID:path/filename.c:LINE"
            match = re.match(r'([A-Z0-9-]+):(.+):(\d+)', title)
            if match:
                rule_id = match.group(1)
                filepath = match.group(2)
                line_num = int(match.group(3))

                # Extract just the filename
                filename = os.path.basename(filepath)
                violations[filename][line_num].append(rule_id)

    return violations

def classify_violations(violations, sections):
    """
    Classify violations as TP/FP based on which section they're in.
    """
    tp_violations = []  # Violations in OMITBAD (good detection)
    fp_violations = []  # Violations in OMITGOOD (false alarm)
    tp_flaw_violations = []  # Violations on FLAW lines (critical detections)

    for line_num, rules in violations.items():
        if line_num in sections['bad_lines']:
            for rule in rules:
                tp_violations.append((line_num, rule))
                if line_num in sections['flaw_lines']:
                    tp_flaw_violations.append((line_num, rule))
        elif line_num in sections['good_lines']:
            for rule in rules:
                fp_violations.append((line_num, rule))

    return tp_violations, fp_violations, tp_flaw_violations

def analyze_single_file(c_filepath, violations_dict):
    """Analyze one C file against its violations."""
    filename = os.path.basename(c_filepath)

    if not os.path.exists(c_filepath):
        return None

    sections = parse_c_file_sections(c_filepath)
    file_violations = violations_dict.get(filename, {})

    if not sections['bad_lines'] and not sections['good_lines']:
        return None  # Not a test case file

    tp_violations, fp_violations, tp_flaw_violations = classify_violations(
        file_violations, sections
    )

    return {
        'filename': filename,
        'bad_lines': len(sections['bad_lines']),
        'good_lines': len(sections['good_lines']),
        'flaw_lines': len(sections['flaw_lines']),
        'flaw_line_set': sections['flaw_lines'],
        'tp_count': len(tp_violations),
        'fp_count': len(fp_violations),
        'tp_flaw_count': len(tp_flaw_violations),
        'tp_violations': tp_violations,
        'fp_violations': fp_violations,
        'tp_flaw_violations': tp_flaw_violations,
        'has_bad_section': bool(sections['bad_lines']),
    }


def _normalize_cwe_id(raw: str) -> str:
    """Normalize CWE ID to 'CWE-NNN' format.

    Handles: 'CWE190', 'CWE-190', 'cwe-190', '190'
    """
    raw = raw.strip().upper()
    if raw.startswith("CWE-"):
        return raw
    if raw.startswith("CWE"):
        return "CWE-" + raw[3:]
    if raw.isdigit():
        return "CWE-" + raw
    return raw


def _extract_cwe_from_dir(dir_path: str) -> str | None:
    """Extract CWE ID from a Juliet directory name.

    E.g. 'CWE190_Integer_Overflow' → 'CWE-190'
         'CWE121_Stack_Based_Buffer_Overflow' → 'CWE-121'
    """
    dirname = Path(dir_path).name
    m = re.match(r'(CWE)(\d+)', dirname)
    if m:
        return f"CWE-{m.group(2)}"
    return None


def _hits_flaw_line(line_num: int, flaw_lines: set) -> bool:
    """Check if a violation line hits a FLAW line with ±1 tolerance."""
    return (line_num in flaw_lines or
            line_num - 1 in flaw_lines or
            line_num + 1 in flaw_lines)


def compute_cwe_aware_metrics(results, cwe_id, cwe_rules):
    """Compute CWE-aware metrics given a set of CWE-matched rule IDs.

    Returns a dict with all CWE-aware metrics, or None if no matched rules.
    """
    if not cwe_rules:
        return None

    cwe_matched_tp = 0
    cwe_matched_fp = 0
    noise_tp = 0
    noise_fp = 0
    cwe_matched_tp_rules = defaultdict(int)
    cwe_matched_fp_rules = defaultdict(int)

    # Per-file detection: did file have ≥1 CWE-relevant TP in OMITBAD?
    files_with_bad_section = 0
    files_detected = 0

    # FLAW-line hit rate: CWE-matched violations within ±1 of a FLAW line
    flaw_hits = 0
    total_flaw_lines = 0

    for r in results:
        if r['has_bad_section']:
            files_with_bad_section += 1

        file_has_cwe_tp = False
        flaw_line_set = r['flaw_line_set']
        total_flaw_lines += len(flaw_line_set)

        for line_num, rule in r['tp_violations']:
            if rule in cwe_rules:
                cwe_matched_tp += 1
                cwe_matched_tp_rules[rule] += 1
                file_has_cwe_tp = True
                if _hits_flaw_line(line_num, flaw_line_set):
                    flaw_hits += 1
            else:
                noise_tp += 1

        for line_num, rule in r['fp_violations']:
            if rule in cwe_rules:
                cwe_matched_fp += 1
                cwe_matched_fp_rules[rule] += 1
            else:
                noise_fp += 1

        if file_has_cwe_tp:
            files_detected += 1

    cwe_matched_total = cwe_matched_tp + cwe_matched_fp
    noise_total = noise_tp + noise_fp
    all_total = cwe_matched_total + noise_total

    return {
        'cwe_id': cwe_id,
        'cwe_rules': sorted(cwe_rules),
        'cwe_matched_tp': cwe_matched_tp,
        'cwe_matched_fp': cwe_matched_fp,
        'cwe_matched_total': cwe_matched_total,
        'cwe_matched_tp_rate': round(cwe_matched_tp / cwe_matched_total * 100, 1) if cwe_matched_total else 0,
        'noise_count': noise_total,
        'noise_ratio': round(noise_total / all_total * 100, 1) if all_total else 0,
        'per_file_detected': files_detected,
        'per_file_total': files_with_bad_section,
        'per_file_rate': round(files_detected / files_with_bad_section * 100, 1) if files_with_bad_section else 0,
        'flaw_hit_detected': flaw_hits,
        'flaw_hit_total': total_flaw_lines,
        'flaw_hit_rate': round(flaw_hits / total_flaw_lines * 100, 1) if total_flaw_lines else 0,
        'cwe_matched_tp_rules': dict(sorted(cwe_matched_tp_rules.items(), key=lambda x: -x[1])),
        'cwe_matched_fp_rules': dict(sorted(cwe_matched_fp_rules.items(), key=lambda x: -x[1])),
    }


def print_cwe_aware_section(metrics):
    """Print the CWE-aware metrics section (appended after existing output)."""
    cwe_id = metrics['cwe_id']
    rules_str = ", ".join(metrics['cwe_rules'])

    print(f"\n--- CWE-Aware Metrics ({cwe_id}) ---")
    print(f"CWE-matched rules: {rules_str}")
    print(f"CWE-matched TP: {metrics['cwe_matched_tp']}")
    print(f"CWE-matched FP: {metrics['cwe_matched_fp']}")
    print(f"CWE-matched TP Rate: {metrics['cwe_matched_tp_rate']}%")
    print(f"Noise findings (non-CWE-matched): {metrics['noise_count']}")
    print(f"Noise ratio: {metrics['noise_ratio']}%")
    print(f"Per-file detection rate: {metrics['per_file_rate']}% ({metrics['per_file_detected']}/{metrics['per_file_total']})")
    print(f"FLAW-line hit rate (CWE-matched): {metrics['flaw_hit_rate']}% ({metrics['flaw_hit_detected']}/{metrics['flaw_hit_total']})")

    print(f"\n--- CWE-Matched Rules in OMITBAD ---")
    for rule, count in metrics['cwe_matched_tp_rules'].items():
        print(f"  {rule}: {count}")

    print(f"\n--- CWE-Matched Rules in OMITGOOD ---")
    for rule, count in metrics['cwe_matched_fp_rules'].items():
        print(f"  {rule}: {count}")


def main():
    import argparse

    parser = argparse.ArgumentParser(description='Analyze SqC results against Juliet ground truth')
    parser.add_argument('--csv', required=True,
                        help='Path to SqC CSV results')
    parser.add_argument('--dir', required=True,
                        help='Path to Juliet test directory (e.g., .../testcases/CWE190_Integer_Overflow)')
    parser.add_argument('--subdir', default=None,
                        help='Specific subdirectory (e.g., s08). If not specified, analyzes all.')
    parser.add_argument('--cwe', default=None,
                        help='CWE ID for CWE-aware filtering (e.g., CWE-190). Auto-detected from --dir if omitted.')
    parser.add_argument('--rule-cwe-map', default=None,
                        help='Path to rule_cwe_map.json for CWE-aware metrics')
    args = parser.parse_args()

    juliet_base = Path(args.dir)
    csv_path = args.csv

    print("Parsing SqC CSV results...")
    violations_dict = parse_sqc_csv(csv_path)
    print(f"Found violations for {len(violations_dict)} files")

    print("\nAnalyzing files...")
    results = []

    # Handle single subdir or all subdirs
    if args.subdir:
        search_dirs = [juliet_base / args.subdir]
    else:
        subdirs = sorted(juliet_base.glob('s*'))
        if subdirs and subdirs[0].is_dir():
            search_dirs = subdirs
        else:
            # Flat layout: .c files directly in the CWE directory
            search_dirs = [juliet_base]

    for search_dir in search_dirs:
        if search_dir.is_dir():
            for c_file in sorted(search_dir.glob('*.c')):
                result = analyze_single_file(c_file, violations_dict)
                if result:
                    results.append(result)

    # Aggregate statistics
    total_tp = sum(r['tp_count'] for r in results)
    total_fp = sum(r['fp_count'] for r in results)
    total_tp_flaw = sum(r['tp_flaw_count'] for r in results)
    total_bad_lines = sum(r['bad_lines'] for r in results)
    total_good_lines = sum(r['good_lines'] for r in results)
    total_flaw_lines = sum(r['flaw_lines'] for r in results)

    # Extract CWE name from directory path
    cwe_name = juliet_base.name  # e.g., "CWE121_Stack_Based_Buffer_Overflow"
    cwe_id = cwe_name.split('_')[0] if '_' in cwe_name else cwe_name

    subdir_desc = f"({args.subdir})" if args.subdir else "(all subdirs)"
    print("\n" + "="*70)
    print(f"JULIET BENCHMARK ANALYSIS - {cwe_id} {subdir_desc}")
    print("="*70)
    print(f"\nFiles analyzed: {len(results)}")
    print(f"Total OMITBAD lines: {total_bad_lines}")
    print(f"Total OMITGOOD lines: {total_good_lines}")
    print(f"Total FLAW comment lines: {total_flaw_lines}")

    print(f"\n--- SqC Violation Distribution ---")
    print(f"Violations in OMITBAD (TP): {total_tp}")
    print(f"Violations in OMITGOOD (FP): {total_fp}")
    print(f"Violations on FLAW lines (TP critical): {total_tp_flaw}")

    # Calculate rates
    total_violations = total_tp + total_fp
    if total_violations > 0:
        fp_rate = (total_fp / total_violations) * 100
        tp_rate = (total_tp / total_violations) * 100
        print(f"\n--- Rates (All Violations) ---")
        print(f"False Positive Rate: {fp_rate:.1f}%")
        print(f"True Positive Rate: {tp_rate:.1f}%")

    # FLAW line detection rate
    if total_flaw_lines > 0:
        flaw_detection_rate = (total_tp_flaw / total_flaw_lines) * 100
        print(f"\n--- Critical Flaw Detection ---")
        print(f"FLAW lines detected: {total_tp_flaw} / {total_flaw_lines}")
        print(f"Detection rate on FLAW lines: {flaw_detection_rate:.1f}%")

    # Most common rules in TP vs FP
    tp_rules = defaultdict(int)
    fp_rules = defaultdict(int)
    tp_flaw_rules = defaultdict(int)

    for r in results:
        for line, rule in r['tp_violations']:
            tp_rules[rule] += 1
        for line, rule in r['fp_violations']:
            fp_rules[rule] += 1
        for line, rule in r['tp_flaw_violations']:
            tp_flaw_rules[rule] += 1

    print(f"\n--- Rules in OMITBAD (True Positives) ---")
    for rule, count in sorted(tp_rules.items(), key=lambda x: x[1], reverse=True):
        print(f"  {rule}: {count}")

    print(f"\n--- Rules in OMITGOOD (False Positives) ---")
    for rule, count in sorted(fp_rules.items(), key=lambda x: x[1], reverse=True):
        print(f"  {rule}: {count}")

    print(f"\n--- Rules on FLAW Lines (Critical Detections) ---")
    for rule, count in sorted(tp_flaw_rules.items(), key=lambda x: x[1], reverse=True):
        print(f"  {rule}: {count}")

    print("\n" + "="*70)

    # ── CWE-Aware Metrics (appended after existing output) ────────────────
    rule_cwe_map_path = args.rule_cwe_map
    cwe_arg = args.cwe

    if rule_cwe_map_path:
        # Load the rule-CWE map
        try:
            with open(rule_cwe_map_path) as f:
                rule_cwe_map = json.load(f)
        except Exception as e:
            print(f"\nWARNING: Could not load rule-CWE map: {e}", file=__import__('sys').stderr)
            return

        # Determine CWE ID: use --cwe if given, else auto-detect from --dir
        if cwe_arg:
            target_cwe = _normalize_cwe_id(cwe_arg)
        else:
            target_cwe = _extract_cwe_from_dir(args.dir)

        if target_cwe:
            cwe_to_rules = rule_cwe_map.get('cwe_to_rules', {})
            matched_rules = set(cwe_to_rules.get(target_cwe, []))

            if matched_rules:
                metrics = compute_cwe_aware_metrics(results, target_cwe, matched_rules)
                if metrics:
                    print_cwe_aware_section(metrics)
            else:
                print(f"\n--- CWE-Aware Metrics ({target_cwe}) ---")
                print(f"No rules mapped to {target_cwe} in rule-CWE map")

if __name__ == '__main__':
    main()
