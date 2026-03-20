#!/usr/bin/env python3
"""Compare old vs new SqC SQLite analysis results to show TP/FP changes."""

import csv
import sys
import re
from collections import defaultdict, Counter
from pathlib import Path

def parse_title(title: str):
    """Extract rule ID from title like 'INT17-C::287 version:abc'"""
    m = re.match(r'^([A-Z]+\d+-C)', title)
    return m.group(1) if m else title.split('::')[0]

def load_csv(path: str) -> list[dict]:
    rows = []
    with open(path, newline='', encoding='utf-8', errors='replace') as f:
        reader = csv.DictReader(f)
        for row in reader:
            rows.append(row)
    return rows

def rule_counts(rows: list[dict]) -> Counter:
    counts = Counter()
    for row in rows:
        rule = parse_title(row.get('Title', ''))
        if rule:
            counts[rule] += 1
    return counts

def main():
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} <old.csv> <new.csv>")
        sys.exit(1)

    old_path, new_path = sys.argv[1], sys.argv[2]
    old_rows = load_csv(old_path)
    new_rows = load_csv(new_path)

    old_counts = rule_counts(old_rows)
    new_counts = rule_counts(new_rows)

    all_rules = sorted(set(old_counts) | set(new_counts))

    total_old = sum(old_counts.values())
    total_new = sum(new_counts.values())

    print(f"{'='*72}")
    print(f"SqC SQLite Analysis Comparison")
    print(f"  Old: {old_path}  ({total_old:,} violations)")
    print(f"  New: {new_path}  ({total_new:,} violations)")
    print(f"  Delta: {total_new - total_old:+,} ({(total_new-total_old)/max(total_old,1)*100:+.1f}%)")
    print(f"{'='*72}")

    # Rules with significant changes
    changes = []
    for rule in all_rules:
        o = old_counts.get(rule, 0)
        n = new_counts.get(rule, 0)
        delta = n - o
        pct = (delta / max(o, 1)) * 100 if o else float('inf')
        changes.append((rule, o, n, delta, pct))

    # Sort by absolute delta descending
    changes.sort(key=lambda x: -abs(x[3]))

    print(f"\n{'Rule':<16} {'Old':>8} {'New':>8} {'Delta':>8} {'%Change':>9}")
    print(f"{'-'*16} {'-'*8} {'-'*8} {'-'*8} {'-'*9}")
    for rule, o, n, delta, pct in changes[:40]:
        pct_str = f"{pct:+.1f}%" if pct != float('inf') else "NEW"
        print(f"{rule:<16} {o:>8,} {n:>8,} {delta:>+8,} {pct_str:>9}")

    # Summary: new rules, removed rules
    new_rules = [r for r in all_rules if old_counts.get(r, 0) == 0]
    removed_rules = [r for r in all_rules if new_counts.get(r, 0) == 0]

    if new_rules:
        print(f"\nNew rules firing: {', '.join(new_rules)}")
    if removed_rules:
        print(f"Rules no longer firing: {', '.join(removed_rules)}")

    # Severity breakdown
    print(f"\n{'='*72}")
    print("Severity breakdown:")
    old_sev = Counter(r.get('Severity','?') for r in old_rows)
    new_sev = Counter(r.get('Severity','?') for r in new_rows)
    all_sevs = sorted(set(old_sev) | set(new_sev))
    for sev in all_sevs:
        o, n = old_sev.get(sev, 0), new_sev.get(sev, 0)
        print(f"  {sev:<20} {o:>8,} -> {n:>8,}  ({n-o:+,})")

if __name__ == '__main__':
    main()
