# Comparison Setup: cppcheck and clang-tidy vs. sqc

This document details how to install **cppcheck** and **clang/clang-tidy** on Ubuntu 24.04, and the commands needed to generate analysis results in formats comparable to sqc output.

---

## Table of Contents

1. [Install cppcheck](#1-install-cppcheck)
2. [Install clang and clang-tidy](#2-install-clang-and-clang-tidy)
3. [Generating cppcheck Results](#3-generating-cppcheck-results)
4. [Generating clang-tidy Results](#4-generating-clang-tidy-results)
5. [Generating sqc Results](#5-generating-sqc-results)
6. [Output Format Reference](#6-output-format-reference)
7. [Juliet Benchmark Comparison](#7-juliet-benchmark-comparison)
8. [Real-World Project Comparisons](#8-real-world-project-comparisons)
   - [8.1 libcrc](#81-libcrc)
   - [8.2 sqlite](#82-sqlite)
   - [8.3 mosquitto](#83-mosquitto)
   - [8.4 curl](#84-curl)
   - [8.5 hostap](#85-hostap)
9. [Notes on Methodology](#9-notes-on-methodology)

---

## 1. Install cppcheck

```bash
sudo apt update
sudo apt install -y cppcheck
```

Verify:

```bash
cppcheck --version
# Expected: Cppcheck 2.x.x
```

Ubuntu 24.04 ships cppcheck 2.13.x via apt. For the latest release, build from source:

```bash
sudo apt install -y cmake libpcre3-dev
git clone https://github.com/danmar/cppcheck.git
cd cppcheck
cmake -DCMAKE_BUILD_TYPE=Release -DUSE_MATCHCOMPILER=ON .
make -j$(nproc)
sudo make install
```

---

## 2. Install clang and clang-tidy

```bash
sudo apt update
sudo apt install -y clang clang-tidy
```

Verify:

```bash
clang --version
# Expected: Ubuntu clang version 18.x.x

clang-tidy --version
# Expected: LLVM version 18.x.x
```

To pin a specific LLVM version (e.g., 18):

```bash
sudo apt install -y clang-18 clang-tidy-18
sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-18 100
sudo update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-18 100
```

---

## 3. Generating cppcheck Results

### 3.1 Basic Run — Text Output

```bash
cppcheck --enable=all --std=c11 /path/to/source/
```

### 3.2 XML Output (for programmatic comparison)

```bash
cppcheck --enable=all --std=c11 --xml /path/to/source/ 2> cppcheck_results.xml
```

Note: cppcheck writes XML to **stderr**, not stdout. Redirect with `2>`.

### 3.3 CERT C Checks Specifically

cppcheck includes a `cert` addon that maps checks to CERT C rules:

```bash
# Using the built-in cert addon
cppcheck --addon=cert --enable=all --std=c11 /path/to/source/ 2>&1 | tee cppcheck_cert.txt

# XML output with cert addon
cppcheck --addon=cert --enable=all --std=c11 --xml /path/to/source/ 2> cppcheck_cert.xml
```

The cert addon file is typically located at:
`/usr/share/cppcheck/addons/cert.py`

### 3.4 Suppress Vendor/Library Headers

```bash
cppcheck \
  --enable=all \
  --std=c11 \
  --suppress=missingIncludeSystem \
  --inline-suppr \
  -i /path/to/source/vendor \
  /path/to/source/
```

### 3.5 Juliet Benchmark Run

```bash
# Single CWE directory
cppcheck \
  --enable=all \
  --std=c11 \
  --addon=cert \
  --xml \
  --xml-version=2 \
  testcases/CWE121_Stack_Based_Buffer_Overflow/ \
  2> cppcheck_CWE121.xml

# All testcases — parallel across CWE directories
find testcases/ -maxdepth 1 -type d -name 'CWE*' | \
  xargs -P 12 -I{} bash -c \
    'cppcheck --enable=all --std=c11 --addon=cert --xml --xml-version=2 "{}" 2> "cppcheck_$(basename {}).xml"'
```

### 3.6 SARIF Output (cppcheck 2.12+)

```bash
cppcheck --enable=all --std=c11 --output-format=sarif /path/to/source/ > cppcheck_results.sarif
```

Note: SARIF output was added in cppcheck 2.12. If your installed version is older, use XML and convert.

### 3.7 Key cppcheck Flags Summary

| Flag | Purpose |
|------|---------|
| `--enable=all` | Enable all checks (style, performance, portability, information, unusedFunction) |
| `--enable=warning` | Only warnings (subset of `all`) |
| `--std=c11` | Target C standard (c89, c99, c11, c17) |
| `--addon=cert` | Enable CERT C rule checks |
| `--xml --xml-version=2` | XML output (to stderr) |
| `--output-format=sarif` | SARIF output (v2.12+) |
| `-j N` | Parallel analysis with N threads |
| `--suppress=ID` | Suppress specific check ID |
| `--inline-suppr` | Honor `// cppcheck-suppress` inline comments |
| `-I /path` | Add include directory |
| `-i /path` | Ignore directory |
| `--force` | Analyze all configurations (slower, more complete) |

---

## 4. Generating clang-tidy Results

### 4.1 Single File

```bash
clang-tidy file.c -- -std=c11
```

### 4.2 Directory Scan

clang-tidy operates on compilation units, not directories. You need a `compile_commands.json` or use a wrapper:

**Option A — With bear (generates compile_commands.json from make):**

```bash
sudo apt install -y bear
bear -- make
clang-tidy -p . src/**/*.c
```

**Option B — Without a build system (pass flags directly):**

```bash
find /path/to/source/ -name '*.c' | \
  xargs -I{} clang-tidy {} -- -std=c11 -I/path/to/source/include
```

**Option C — run-clang-tidy wrapper (parallel):**

```bash
# Requires compile_commands.json already generated
run-clang-tidy -j $(nproc) -p /build/dir/
```

### 4.3 CERT C Checks with clang-tidy

```bash
clang-tidy \
  -checks='-*,cert-*' \
  file.c \
  -- -std=c11
```

Enable specific CERT categories:

```bash
# Array/string safety
clang-tidy -checks='-*,cert-arr*,cert-str*' file.c -- -std=c11

# All CERT checks plus clang-analyzer
clang-tidy -checks='-*,cert-*,clang-analyzer-*' file.c -- -std=c11
```

### 4.4 SARIF / JSON Output

```bash
# Export as JSON (one object per diagnostic)
clang-tidy -checks='-*,cert-*' --export-fixes=fixes.yaml file.c -- -std=c11

# SARIF output (clang-tidy 16+)
clang-tidy -checks='-*,cert-*' --output-format=sarif file.c -- -std=c11 > clang_tidy.sarif
```

### 4.5 Juliet Benchmark Run

Juliet C files can be analyzed individually since they are self-contained translation units:

```bash
# Single CWE directory
find testcases/CWE121_Stack_Based_Buffer_Overflow/ -name '*.c' | \
  xargs -P 12 -I{} clang-tidy \
    -checks='-*,cert-*,clang-analyzer-*' \
    {} \
    -- -std=c11 -I testcasesupport/ \
  2>&1 > clang_tidy_CWE121.txt

# SARIF per file (clang-tidy 16+)
find testcases/CWE121_Stack_Based_Buffer_Overflow/ -name '*.c' | \
  while read f; do
    clang-tidy \
      -checks='-*,cert-*,clang-analyzer-*' \
      --output-format=sarif \
      "$f" \
      -- -std=c11 -I testcasesupport/
  done > clang_tidy_CWE121.sarif
```

### 4.6 Key clang-tidy Flags Summary

| Flag | Purpose |
|------|---------|
| `-checks='...'` | Comma-separated check patterns (`-*` = disable all first) |
| `-checks='-*,cert-*'` | Enable only CERT checks |
| `-checks='-*,clang-analyzer-*'` | Enable only Clang Static Analyzer checks |
| `--export-fixes=FILE` | Write fixes to YAML |
| `--output-format=sarif` | SARIF output (clang-tidy 16+) |
| `-p /build/dir/` | Path to `compile_commands.json` directory |
| `-j N` | Parallel run-clang-tidy threads |
| `--` | Separator; flags after apply to compiler |
| `-std=c11` | C standard for compiler |
| `-I /path` | Include directory for compiler |
| `-DOMITBAD` / `-DOMITGOOD` | Juliet preprocessor guards |

---

## 5. Generating sqc Results

### 5.1 Standard Run

```bash
# Build first (if not already built)
cargo build --release

# Basic stdout output
./target/release/sqc /path/to/source/

# Export to JSON
./target/release/sqc /path/to/source/ --export results.json

# Export to SARIF
./target/release/sqc /path/to/source/ --export results.sarif

# Export to CSV
./target/release/sqc /path/to/source/ --export results.csv
```

### 5.2 Juliet Benchmark Run (matches BENCHMARK.md methodology)

```bash
# Single CWE directory with cross-file context
./target/release/sqc testcases/CWE121_Stack_Based_Buffer_Overflow/ \
  -d testcases/ \
  -d testcasesupport/ \
  --export results_CWE121.csv

# All CWE directories — parallel (12 jobs)
find testcases/ -maxdepth 1 -type d -name 'CWE*' | \
  xargs -P 12 -I{} bash -c \
    './target/release/sqc "{}" \
      -d testcases/ -d testcasesupport/ \
      --export "results_$(basename {}).csv"'
```

### 5.3 Filtered Output for Comparison

```bash
# Only high severity
./target/release/sqc /path/to/source/ \
  --min-severity High \
  --export results_high.json

# Specific rules only
./target/release/sqc /path/to/source/ \
  --rules ARR30-C,MEM30-C,STR31-C \
  --export results_memory.json
```

### 5.4 SARIF for CI/CD Integration

```bash
./target/release/sqc /path/to/source/ \
  -d /path/to/source/ \
  --min-severity Medium \
  --fail-on-severity High \
  --export results.sarif
```

---

## 6. Output Format Reference

### cppcheck XML (v2)

```xml
<?xml version="1.0" encoding="UTF-8"?>
<results version="2">
  <cppcheck version="2.13.0"/>
  <errors>
    <error id="bufferAccessOutOfBounds" severity="error"
           msg="Array 'arr[10]' accessed at index 10..." cwe="788">
      <location file="example.c" line="5" column="5"/>
    </error>
  </errors>
</results>
```

Key fields: `id` (check name), `severity`, `msg`, `cwe`, `file`, `line`.

### clang-tidy Text Output

```
example.c:5:5: warning: ... [cert-arr30-c]
    arr[10] = 0;
    ^
```

Key fields: `file:line:col`, `severity`, message, `[check-id]`.

### sqc JSON Output

```json
[
  {
    "rule_id": "ARR30-C",
    "severity": "High",
    "file": "example.c",
    "line": 5,
    "column": 5,
    "message": "Out-of-bounds array access",
    "category": "ARR"
  }
]
```

Key fields: `rule_id`, `severity`, `file`, `line`, `column`, `message`, `category`.

### sqc SARIF Output

sqc produces SARIF 2.1.0 compatible output with `ruleId` matching CERT C rule IDs (e.g., `ARR30-C`).

---

## 7. Juliet Benchmark Comparison

To compare tool results against the same Juliet ground truth used in `BENCHMARK.md`:

### Ground Truth Classification

Juliet uses preprocessor guards to mark code sections:
- `#ifndef OMITBAD` — vulnerable code → violations here are **True Positives**
- `#ifndef OMITGOOD` — fixed code → violations here are **False Positives**

### Running All Three Tools on the Same Corpus

```bash
JULIET_DIR=testcases
SUPPORT_DIR=testcasesupport

# --- sqc ---
find "$JULIET_DIR" -maxdepth 1 -type d -name 'CWE*' | \
  xargs -P 12 -I{} bash -c \
    './target/release/sqc "{}" -d "$JULIET_DIR" -d "$SUPPORT_DIR" \
      --export "results/sqc/$(basename {}).csv"'

# --- cppcheck ---
find "$JULIET_DIR" -maxdepth 1 -type d -name 'CWE*' | \
  xargs -P 12 -I{} bash -c \
    'cppcheck --enable=all --std=c11 --addon=cert --xml --xml-version=2 "{}" \
      2> "results/cppcheck/$(basename {}).xml"'

# --- clang-tidy ---
find "$JULIET_DIR" -maxdepth 1 -type d -name 'CWE*' | while read dir; do
  find "$dir" -name '*.c' | \
    xargs -P 12 -I{} clang-tidy \
      -checks='-*,cert-*,clang-analyzer-*' \
      {} \
      -- -std=c11 -I "$SUPPORT_DIR" \
    >> "results/clang-tidy/$(basename "$dir").txt" 2>&1
done
```

### Classifying Results with the Analysis Script

```bash
# sqc results (uses existing script)
python3 scripts/analyze_juliet_results.py results/sqc/

# For cppcheck/clang-tidy you will need to adapt the script
# or write a converter that maps their output to the same
# (file, line, bad_section, good_section) classification used for sqc.
```

### Expected Performance Comparison

Based on published benchmarks and BENCHMARK.md data:

| Tool | Juliet TP Rate | FP Rate | CERT C Coverage |
|------|---------------|---------|-----------------|
| sqc (Round 9) | ~43.8% | ~56.2% | 283 rules |
| cppcheck (cert addon) | Low (~10-15%) | Very low | Subset of rules |
| clang-tidy (cert-\*) | Low–moderate | Low | Subset of rules |
| Semgrep CE | ~44-48% | Very low | Pattern-based |
| Infer | ~55% | ~45% | Flow-sensitive |

Note: cppcheck and clang-tidy prioritize precision (low FP) over recall; their Juliet TP rates are low because they implement only a subset of CERT C rules and require precise pattern matches.

---

## 8. Real-World Project Comparisons

All commands below assume:
- sqc is built at `~/data/tools_sqc/target/release/sqc`
- Results are written to `~/data/comparisons/results/{tool}/{project}/`
- `bear` is installed (`sudo apt install -y bear`) for projects that need `compile_commands.json`
- `run-clang-tidy` is available (`sudo apt install -y clang-tidy python3-clang-tidy` or via llvm package)

```bash
# Create output directories once
mkdir -p ~/data/comparisons/results/{sqc,cppcheck,clang-tidy}/{libcrc,sqlite,mosquitto,curl,hostap}
```

---

### 8.1 libcrc

**Project**: `~/data/comparisons/libcrc`
**Build system**: Plain Makefile
**Source**: `src/` (9 `.c` files)
**Headers**: `include/`

libcrc is the simplest of the five projects — no configure step, no external dependencies.

#### sqc

```bash
~/data/tools_sqc/target/release/sqc ~/data/comparisons/libcrc \
  -d ~/data/comparisons/libcrc \
  --export ~/data/comparisons/results/sqc/libcrc/results.json
```

#### cppcheck

```bash
cppcheck \
  --enable=all \
  --std=c11 \
  --addon=cert \
  --xml --xml-version=2 \
  -I ~/data/comparisons/libcrc/include \
  ~/data/comparisons/libcrc/src/ \
  2> ~/data/comparisons/results/cppcheck/libcrc/results.xml
```

#### clang-tidy

libcrc uses a plain Makefile with no configure, so use `bear` to capture the compilation database, or scan files directly (include path is simple enough):

```bash
# Option A — bear (recommended, captures exact compiler flags)
cd ~/data/comparisons/libcrc && bear -- make
run-clang-tidy \
  -clang-tidy-binary clang-tidy \
  -checks='-*,cert-*,clang-analyzer-*' \
  -p ~/data/comparisons/libcrc \
  2>&1 | tee ~/data/comparisons/results/clang-tidy/libcrc/results.txt

# Option B — direct file scan (no build required)
find ~/data/comparisons/libcrc/src/ -name '*.c' | \
  xargs -P $(nproc) -I{} clang-tidy \
    -checks='-*,cert-*,clang-analyzer-*' \
    {} \
    -- -std=c11 -I ~/data/comparisons/libcrc/include \
  2>&1 | tee ~/data/comparisons/results/clang-tidy/libcrc/results.txt
```

---

### 8.2 sqlite

**Project**: `~/data/comparisons/sqlite`
**Build system**: autoconf (`configure` script)
**Source**: `src/` (~70 `.c` files)

sqlite uses extensive preprocessor conditionals and internal macros. Expect higher FP rates from all tools due to unexpanded macros. There is no amalgamation (`sqlite3.c`) in this checkout — analysis targets the split source tree.

#### sqc

```bash
~/data/tools_sqc/target/release/sqc ~/data/comparisons/sqlite \
  -d ~/data/comparisons/sqlite \
  --export ~/data/comparisons/results/sqc/sqlite/results.json
```

#### cppcheck

```bash
cppcheck \
  --enable=all \
  --std=c11 \
  --addon=cert \
  --xml --xml-version=2 \
  --suppress=missingIncludeSystem \
  -I ~/data/comparisons/sqlite/src \
  ~/data/comparisons/sqlite/src/ \
  2> ~/data/comparisons/results/cppcheck/sqlite/results.xml
```

#### clang-tidy

sqlite's configure script generates the Makefile; use `bear` to intercept the build:

```bash
cd ~/data/comparisons/sqlite
./configure
bear -- make -j$(nproc)
run-clang-tidy \
  -clang-tidy-binary clang-tidy \
  -checks='-*,cert-*,clang-analyzer-*' \
  -p ~/data/comparisons/sqlite \
  2>&1 | tee ~/data/comparisons/results/clang-tidy/sqlite/results.txt
```

If `configure` fails due to missing dependencies, fall back to direct file scan:

```bash
find ~/data/comparisons/sqlite/src/ -name '*.c' | \
  xargs -P $(nproc) -I{} clang-tidy \
    -checks='-*,cert-*,clang-analyzer-*' \
    {} \
    -- -std=c11 -I ~/data/comparisons/sqlite/src \
  2>&1 | tee ~/data/comparisons/results/clang-tidy/sqlite/results.txt
```

---

### 8.3 mosquitto

**Project**: `~/data/comparisons/mosquitto`
**Build system**: CMake
**Source**: `lib/` (client library), `src/` (broker)

#### sqc

```bash
~/data/tools_sqc/target/release/sqc ~/data/comparisons/mosquitto \
  -d ~/data/comparisons/mosquitto \
  --export ~/data/comparisons/results/sqc/mosquitto/results.json
```

#### cppcheck

```bash
cppcheck \
  --enable=all \
  --std=c11 \
  --addon=cert \
  --xml --xml-version=2 \
  --suppress=missingIncludeSystem \
  -I ~/data/comparisons/mosquitto/include \
  -I ~/data/comparisons/mosquitto/common \
  -i ~/data/comparisons/mosquitto/deps \
  ~/data/comparisons/mosquitto/lib/ \
  ~/data/comparisons/mosquitto/src/ \
  2> ~/data/comparisons/results/cppcheck/mosquitto/results.xml
```

#### clang-tidy

CMake natively exports `compile_commands.json` with one flag:

```bash
cmake \
  -S ~/data/comparisons/mosquitto \
  -B ~/data/comparisons/mosquitto/build \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DWITH_TLS=OFF \
  -DWITH_WEBSOCKETS=OFF

cmake --build ~/data/comparisons/mosquitto/build -j$(nproc)

run-clang-tidy \
  -clang-tidy-binary clang-tidy \
  -checks='-*,cert-*,clang-analyzer-*' \
  -p ~/data/comparisons/mosquitto/build \
  2>&1 | tee ~/data/comparisons/results/clang-tidy/mosquitto/results.txt
```

`-DWITH_TLS=OFF -DWITH_WEBSOCKETS=OFF` avoids failures if OpenSSL or libwebsockets headers are absent. Remove those flags if the libraries are installed.

---

### 8.4 curl

**Project**: `~/data/comparisons/curl`
**Build system**: CMake (preferred) or autoconf
**Source**: `lib/` (libcurl, ~200 `.c` files), `src/` (curl CLI)

#### sqc

```bash
~/data/tools_sqc/target/release/sqc ~/data/comparisons/curl \
  -d ~/data/comparisons/curl \
  --export ~/data/comparisons/results/sqc/curl/results.json
```

#### cppcheck

```bash
cppcheck \
  --enable=all \
  --std=c11 \
  --addon=cert \
  --xml --xml-version=2 \
  --suppress=missingIncludeSystem \
  -I ~/data/comparisons/curl/include \
  -I ~/data/comparisons/curl/lib \
  ~/data/comparisons/curl/lib/ \
  ~/data/comparisons/curl/src/ \
  2> ~/data/comparisons/results/cppcheck/curl/results.xml
```

#### clang-tidy

```bash
cmake \
  -S ~/data/comparisons/curl \
  -B ~/data/comparisons/curl/build \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DBUILD_SHARED_LIBS=OFF \
  -DCURL_USE_OPENSSL=OFF \
  -DCURL_DISABLE_LDAP=ON

cmake --build ~/data/comparisons/curl/build -j$(nproc)

run-clang-tidy \
  -clang-tidy-binary clang-tidy \
  -checks='-*,cert-*,clang-analyzer-*' \
  -p ~/data/comparisons/curl/build \
  2>&1 | tee ~/data/comparisons/results/clang-tidy/curl/results.txt
```

`-DCURL_USE_OPENSSL=OFF -DCURL_DISABLE_LDAP=ON` avoids missing-library failures in a minimal environment. For a full analysis with TLS enabled, install `libssl-dev` and drop those flags.

---

### 8.5 hostap

**Project**: `~/data/comparisons/hostap`
**Build system**: Makefile (wpa_supplicant and hostapd each have their own)
**Source**: `src/` (shared), `wpa_supplicant/`, `hostapd/`

hostap ships `gen_compile_commands.py` which reads build artefacts to produce `compile_commands.json` — no need for `bear`.

#### sqc

```bash
# Analyze shared source + wpa_supplicant
~/data/tools_sqc/target/release/sqc ~/data/comparisons/hostap \
  -d ~/data/comparisons/hostap/src \
  -d ~/data/comparisons/hostap/wpa_supplicant \
  --export ~/data/comparisons/results/sqc/hostap/results.json
```

#### cppcheck

```bash
cppcheck \
  --enable=all \
  --std=c11 \
  --addon=cert \
  --xml --xml-version=2 \
  --suppress=missingIncludeSystem \
  -I ~/data/comparisons/hostap/src \
  -I ~/data/comparisons/hostap/src/utils \
  -I ~/data/comparisons/hostap/src/common \
  ~/data/comparisons/hostap/src/ \
  ~/data/comparisons/hostap/wpa_supplicant/ \
  2> ~/data/comparisons/results/cppcheck/hostap/results.xml
```

#### clang-tidy

hostap's build requires a `.config` file before make will run. A minimal config for wpa_supplicant that avoids external library dependencies:

```bash
cd ~/data/comparisons/hostap/wpa_supplicant

# Create a minimal build config
cat > .config <<'EOF'
CONFIG_DRIVER_NL80211=y
CONFIG_LIBNL32=y
CONFIG_IEEE8021X_EAPOL=y
CONFIG_EAP_MD5=y
EOF

# Build to generate object files (needed by gen_compile_commands.py)
bear -- make -j$(nproc) wpa_supplicant 2>/dev/null || true

# Generate compile_commands.json using hostap's own script
cd ~/data/comparisons/hostap
python3 gen_compile_commands.py \
  -o compile_commands.json \
  wpa_supplicant/

run-clang-tidy \
  -clang-tidy-binary clang-tidy \
  -checks='-*,cert-*,clang-analyzer-*' \
  -p ~/data/comparisons/hostap \
  2>&1 | tee ~/data/comparisons/results/clang-tidy/hostap/results.txt
```

If the build fails due to missing `libnl` or kernel headers, fall back to a direct file scan of the shared source:

```bash
find ~/data/comparisons/hostap/src/ -name '*.c' | \
  xargs -P $(nproc) -I{} clang-tidy \
    -checks='-*,cert-*,clang-analyzer-*' \
    {} \
    -- -std=c11 \
       -I ~/data/comparisons/hostap/src \
       -I ~/data/comparisons/hostap/src/utils \
       -I ~/data/comparisons/hostap/src/common \
       -I ~/data/comparisons/hostap/src/crypto \
  2>&1 | tee ~/data/comparisons/results/clang-tidy/hostap/results.txt
```

---

## 9. Notes on Methodology

### Apples-to-Apples Concerns

1. **Rule coverage**: cppcheck `cert` addon and clang-tidy `cert-*` checks implement a small subset of the 283 CERT C rules sqc covers. Raw violation counts are not directly comparable.

2. **Translation unit scope**: cppcheck and clang-tidy are per-file by default. sqc supports cross-file analysis via `-d`. Use consistent scope when comparing.

3. **Preprocessor handling**: cppcheck evaluates all `#ifdef` configurations by default (use `--force`). clang-tidy sees one configuration. sqc uses tree-sitter and does not expand macros — all branches visible in source are analyzed. For Juliet, compile with `-DOMITBAD` or `-DOMITGOOD` when needed.

4. **Standard library awareness**: cppcheck and clang-tidy have built-in knowledge of standard library semantics. sqc uses a curated `std_functions.rs` database (~270 functions).

5. **Severity mapping**: cppcheck uses `error/warning/style/performance/portability`; clang-tidy uses `error/warning/note`; sqc uses `Low/Medium/High/Critical`. Map conservatively when cross-comparing.

### Recommended Comparison Workflow

1. Pick a representative C codebase or CWE subset from Juliet.
2. Run all three tools with consistent flags.
3. Normalize output to `(file, line, rule/check-id)` tuples.
4. Classify each finding as TP or FP using Juliet's OMITBAD/OMITGOOD sections.
5. Compute precision, recall, and F1 per tool.
6. Restrict comparison to rules/checks that all tools implement for fair overlap analysis.
