#!/usr/bin/env python3
"""
Analyze SqC results against Juliet ground truth to calculate:
- True Positive Rate (TPR)
- False Positive Rate (FPR)
- False Negative Rate (FNR)
"""

import csv
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
        'tp_count': len(tp_violations),
        'fp_count': len(fp_violations),
        'tp_flaw_count': len(tp_flaw_violations),
        'tp_violations': tp_violations,
        'fp_violations': fp_violations,
        'tp_flaw_violations': tp_flaw_violations
    }

def main():
    # Paths
    juliet_dir = Path.home() / 'data/benchmarks/juliet-test-suite-c/testcases/CWE121_Stack_Based_Buffer_Overflow/s08'
    csv_path = '/tmp/juliet_cwe121_s08.csv'

    print("Parsing SqC CSV results...")
    violations_dict = parse_sqc_csv(csv_path)
    print(f"Found violations for {len(violations_dict)} files")

    print("\nAnalyzing files...")
    results = []

    for c_file in sorted(juliet_dir.glob('*.c')):
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

    print("\n" + "="*70)
    print("JULIET BENCHMARK ANALYSIS - CWE-121 (s08 subset)")
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

    print(f"\n--- Top 10 Rules in OMITBAD (True Positives) ---")
    for rule, count in sorted(tp_rules.items(), key=lambda x: x[1], reverse=True)[:10]:
        print(f"  {rule}: {count}")

    print(f"\n--- Top 10 Rules in OMITGOOD (False Positives) ---")
    for rule, count in sorted(fp_rules.items(), key=lambda x: x[1], reverse=True)[:10]:
        print(f"  {rule}: {count}")

    print(f"\n--- Top Rules on FLAW Lines (Critical Detections) ---")
    for rule, count in sorted(tp_flaw_rules.items(), key=lambda x: x[1], reverse=True)[:10]:
        print(f"  {rule}: {count}")

    print("\n" + "="*70)

if __name__ == '__main__':
    main()
