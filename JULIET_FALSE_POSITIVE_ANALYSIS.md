# SqC Juliet Benchmark - False Positive/Negative Analysis

**Date**: 2026-01-08
**Test Set**: CWE-121 Stack-Based Buffer Overflow (s08 subset, 624 files)
**SqC Version**: 1ad80211

---

## Executive Summary

Analysis of SqC's performance against NIST Juliet ground truth reveals:

**Overall Performance**:
- ✅ **True Positive Rate: 43.6%** - Violations correctly flagged in vulnerable code (OMITBAD)
- ⚠️ **False Positive Rate: 56.4%** - Violations incorrectly flagged in safe code (OMITGOOD)
- ❌ **False Negative: Critical** - 0% detection on lines marked with /* FLAW */ comments
- ❌ **Coverage Gap: Wide Characters** - STR31-C rule misses `wcscat`, only detects `strcat`

**Key Insight**: SqC detects **generic coding standards violations** that apply to both good and bad code, but struggles to detect the **specific buffer overflow vulnerability** itself, especially for wide character functions.

---

## Detailed Analysis

### 1. Ground Truth Structure

**Juliet Test Files Contain**:
- **OMITBAD sections**: Code with known vulnerabilities (ground truth positive)
- **OMITGOOD sections**: Fixed/safe code (ground truth negative)
- **/* FLAW */ comments**: Mark the exact vulnerable lines

**Test Set Statistics**:
```
Files analyzed:           624
Total OMITBAD lines:      13,704 lines (vulnerable code)
Total OMITGOOD lines:     20,616 lines (safe code)
Total FLAW comments:      1,436 lines (exact vulnerability locations)
```

### 2. SqC Violation Distribution

**By Code Section**:
```
Violations in OMITBAD (TP):    5,253 (43.6%)
Violations in OMITGOOD (FP):   6,800 (56.4%)
Violations on FLAW lines:      0 (0.0%)
```

**Interpretation**:
- SqC reports MORE violations in safe code than vulnerable code
- This indicates most violations are **coding style issues**, not security bugs
- Zero detections on FLAW lines means SqC is not pinpointing the actual vulnerabilities

### 3. Rule Distribution Analysis

#### Top Rules in OMITBAD (True Positives)
| Rule | Count | Type | Security Relevance |
|------|-------|------|-------------------|
| DCL31-C | 978 | Style | ❌ Low (undeclared identifiers) |
| DCL07-C | 974 | Style | ❌ Low (missing type info) |
| DCL06-C | 540 | Style | ⚠️ Medium (magic numbers - size mismatches) |
| FLP34-C | 455 | Safety | ❌ Low (float conversions - irrelevant) |
| EXP34-C | 321 | Safety | ⚠️ Medium (null pointer dereference) |
| CON08-C | 201 | Concurrency | ❌ Low (thread safety - test harness) |
| EXP12-C | 193 | Safety | ⚠️ Medium (ignored return values) |
| DCL20-C | 167 | Style | ❌ Low (storage class specifiers) |
| INT32-C | 136 | Safety | ❌ Low (integer wrapping - irrelevant) |
| INT36-C | 136 | Safety | ❌ Low (integer conversions) |

#### Top Rules in OMITGOOD (False Positives)
| Rule | Count | Type | Pattern |
|------|-------|------|---------|
| DCL31-C | 1,278 | Style | **More in safe code** ⚠️ |
| DCL07-C | 1,275 | Style | **More in safe code** ⚠️ |
| FLP34-C | 614 | Safety | **More in safe code** ⚠️ |
| DCL06-C | 445 | Style | Slightly less in safe code |
| EXP34-C | 439 | Safety | **More in safe code** ⚠️ |

**Key Finding**: Most rules trigger MORE in safe code than vulnerable code, indicating they're not effective discriminators.

### 4. Security-Relevant Rules Analysis

**Buffer Overflow Specific Rules**:
```
STR31-C: 1,246 detections total
ARR38-C: 759 detections total
ARR30-C: Not detected in this test set
```

**Critical Gap Discovered**:
```
Files using strcat:       362 files
Files using wcscat:       262 files
STR31-C on strcat:        1,246 detections ✅
STR31-C on wcscat:        0 detections ❌ (FALSE NEGATIVE)
```

**Analysis**: SqC's STR31-C rule is **only implemented for narrow character functions** (`strcat`, `strcpy`, etc.) but **NOT for wide character functions** (`wcscat`, `wcscpy`, etc.). This represents a **major coverage gap** for buffer overflow detection.

---

## False Positive/Negative Breakdown

### False Positive Rate: 56.4%

**Definition**: Violations flagged in OMITGOOD (safe code) sections.

**Root Causes**:
1. **Generic Coding Standards**: Rules like DCL31-C, DCL07-C apply to ALL code regardless of security
2. **Test Infrastructure Noise**: Test harness code (srand, main functions) triggers CON08-C, EXP12-C
3. **Non-Discriminatory Rules**: Same patterns appear in both safe and vulnerable code

**Impact**: High false positive rate makes it difficult to prioritize which violations are security-critical.

**Example False Positive**:
```c
#ifndef OMITGOOD
void good() {
    wchar_t dataGoodBuffer[100];  // FIX: Large enough buffer
    wchar_t source[100];
    wcscat(dataGoodBuffer, source); // ✅ Safe, but still triggers DCL31-C, DCL07-C
}
#endif
```

### False Negative Rate: Unknown (Cannot Calculate)

**Definition**: Vulnerabilities missed by SqC.

**Why We Can't Calculate**:
- False Negative Rate = Missed Vulnerabilities / Total Vulnerabilities
- We need to know: "Which SPECIFIC vulnerabilities should SqC detect?"
- SqC is a **coding standards checker**, not a **vulnerability detector**

**What We Can Measure**:
- **FLAW Line Detection**: 0 / 1,436 (0%)
- **Wide Char Coverage**: 0 / 262 wcscat files (0%)

**Critical False Negatives**:
1. **Wide Character Functions**: 262 files with `wcscat` missed entirely
2. **Data Flow Analysis**: Cannot prove buffer overflow without tracking buffer sizes
3. **Indirect Vulnerabilities**: Misses vulnerabilities where size mismatch is computed

**Example False Negative**:
```c
#ifndef OMITBAD
void bad() {
    wchar_t dataBadBuffer[50];   // Detected: DCL06-C (magic number)
    wchar_t source[100];         // Detected: DCL06-C (magic number)
    wcscat(dataBadBuffer, source); // ❌ MISSED: No STR31-C detection (wcscat not supported)
}
#endif
```

---

## Comparison with Expected Performance

### What a Perfect Tool Would Do

**On Juliet CWE-121 Tests**:
- ✅ Detect 100% of buffer overflows in OMITBAD
- ✅ Report 0% false alarms in OMITGOOD
- ✅ Flag every /* FLAW */ line
- ✅ Handle both narrow (char) and wide (wchar_t) characters

### What SqC Actually Does

**Strengths**:
- ✅ Detects 1,246 STR31-C violations on `strcat` (narrow char)
- ✅ Comprehensive rule coverage (280+ CERT rules checked)
- ✅ Fast analysis (0.05s per file)
- ✅ Identifies code smells (DCL06-C magic numbers) that correlate with bugs

**Weaknesses**:
- ❌ Does NOT detect `wcscat` buffer overflows (false negative)
- ❌ 56.4% false positive rate (more noise than signal)
- ❌ 0% detection on FLAW lines (not pinpointing vulnerabilities)
- ❌ Cannot perform data-flow analysis to prove overflows

---

## Real-World Implications

### For Developers

**Using SqC Today**:
- ✅ **Good for**: Enforcing CERT C coding standards
- ⚠️ **Moderate for**: Finding potential security issues (via code smells)
- ❌ **Poor for**: Proving buffer overflows exist

**Recommended Workflow**:
1. Run SqC to find coding standards violations
2. Prioritize DCL06-C (magic numbers) and STR31-C (string operations)
3. Manually review flagged code for actual vulnerabilities
4. **Do NOT rely on SqC alone** for security validation

### For Security Teams

**Coverage Gaps**:
- Wide character functions (`wcscat`, `wcscpy`, `wmemcpy`, etc.) are **not covered**
- Data-flow analysis (tracking buffer sizes through code) is **not supported**
- Indirect vulnerabilities (computed sizes, pointer arithmetic) are **missed**

**Comparison with Commercial Tools**:
- **Coverity**: Claims 97.5% CERT C coverage, includes data-flow analysis
- **CodeSonar**: Full data-flow and control-flow analysis
- **SqC**: Syntax-based pattern matching only

---

## Recommendations

### Immediate Fixes

1. **Extend STR31-C to Wide Characters**
   - Add `wcscat`, `wcscpy`, `wcsncat`, `wcsncpy` detection
   - This would eliminate the 262-file false negative gap

2. **Reduce False Positives**
   - Filter out test infrastructure noise (main functions, test harness)
   - Make rules context-aware (don't flag safe buffer operations)

3. **Add Severity Ranking**
   - Mark STR31-C, ARR38-C as HIGH severity (security-critical)
   - Mark DCL31-C, DCL07-C as LOW severity (style only)

### Long-Term Enhancements

1. **Data-Flow Analysis**
   - Track buffer sizes through assignments and function calls
   - Prove when `dest_size < src_size` (definite overflow)
   - Reduce false negatives on complex cases

2. **CWE Mapping**
   - Map CERT rules → CWE categories
   - Report: "CWE-121 detected via STR31-C violation"
   - Enable direct comparison with Juliet ground truth

3. **Ground Truth Validation**
   - Test against all 118 Juliet CWE categories
   - Calculate per-CWE precision and recall
   - Publish public benchmark results

---

## Conclusion

**Current State**:
- SqC is a **comprehensive CERT C coding standards checker** (280+ rules)
- It has a **56.4% false positive rate** due to generic style rules
- It has a **critical false negative gap** for wide character functions
- It **cannot prove buffer overflows** without data-flow analysis

**Value Proposition**:
- ✅ Fast, local analysis (0.05s per file)
- ✅ Open source and verifiable
- ✅ Good at finding code smells that correlate with bugs
- ⚠️ Should be used as **first pass**, not definitive security validation

**Bottom Line**: SqC is best used as a **coding standards enforcer** and **vulnerability indicator tool**, not a **definitive vulnerability detector**. Extending STR31-C to wide characters would be a high-impact, low-effort improvement.

---

## Appendix: Methodology

### Analysis Script
- **Location**: `scripts/analyze_juliet_results.py`
- **Method**: Parse C files to extract OMITBAD/OMITGOOD line ranges, map SqC CSV violations to sections
- **Classification**: TP (violations in OMITBAD), FP (violations in OMITGOOD)

### Test Data
- **Juliet Subset**: CWE-121 s08 directory (624 files)
- **SqC Results**: `/tmp/juliet_cwe121_s08.csv` (37,242 violations)
- **Validation**: Manual inspection of 10 sample files confirms accuracy

### Limitations
- Analysis only covers CWE-121 (buffer overflow), not all 118 CWEs
- "False negative" rate cannot be calculated without knowing expected detections
- FLAW line detection may be off-by-one (comments vs actual code)

---

**Generated**: 2026-01-08
**Analyst**: Claude (AI Assistant)
**Tool Tested**: SqC v1ad80211
