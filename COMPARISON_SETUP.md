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
9. [Verifying Results Are Valid](#9-verifying-results-are-valid)
   - [9.1 Known Pitfalls and Fixes](#91-known-pitfalls-and-fixes)
   - [9.2 Verification Scripts](#92-verification-scripts)
   - [9.3 libcrc Baseline Reference](#93-libcrc-baseline-reference)
   - [9.4 curl Baseline Reference](#94-curl-baseline-reference)
   - [9.5 mosquitto Baseline Reference](#95-mosquitto-baseline-reference)
10. [Notes on Methodology](#10-notes-on-methodology)
11. [Distributed Benchmarking with GNU Parallel (8-Node Cluster)](#11-distributed-benchmarking-with-gnu-parallel-8-node-cluster)
    - [11.1 Prerequisites](#111-prerequisites)
    - [11.2 Node File](#112-node-file)
    - [11.3 Fast Re-Benchmark Workflow](#113-fast-re-benchmark-workflow)
    - [11.4 Juliet Benchmark Distribution](#114-juliet-benchmark-distribution)
    - [11.5 Real-World Project Distribution](#115-real-world-project-distribution)
    - [11.6 Scaling to More Projects](#116-scaling-to-more-projects)
12. [TODO: Additional Tools to Evaluate](#12-todo-additional-tools-to-evaluate)

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

### 3.3 CERT C Checks

In cppcheck 2.x, CERT C checks are built-in and activated by `--enable=all`. The `cert.py` addon from cppcheck 1.x is no longer included in Ubuntu 24.04 packages and is not needed:

```bash
# CERT C checks are included in --enable=all (no addon required)
cppcheck --enable=all --std=c11 /path/to/source/ 2>&1 | tee cppcheck_cert.txt

# XML output
cppcheck --enable=all --std=c11 --xml /path/to/source/ 2> cppcheck_cert.xml
```

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
  --xml \
  --xml-version=2 \
  testcases/CWE121_Stack_Based_Buffer_Overflow/ \
  2> cppcheck_CWE121.xml

# All testcases — parallel across CWE directories
find testcases/ -maxdepth 1 -type d -name 'CWE*' | \
  xargs -P 12 -I{} bash -c \
    'cppcheck --enable=all --std=c11 --xml --xml-version=2 "{}" 2> "cppcheck_$(basename {}).xml"'
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
| `--addon=misra` | Enable MISRA C checks (available in Ubuntu 24.04 package) |
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

The default manifest path (`rules_templates/rules-all.toml`) is resolved relative to the current working directory. Run from within the sqc repo, or pass `--manifest` explicitly when running from elsewhere.

```bash
# Build first (if not already built)
cargo build --release

# Basic stdout output (run from sqc repo root)
./target/release/sqc /path/to/source/

# Basic stdout output from any directory
/path/to/sqc /path/to/source/ --manifest /path/to/sqc-repo/rules_templates/rules-all.toml

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
    'cppcheck --enable=all --std=c11 --xml --xml-version=2 "{}" \
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

**sqc manifest note**: sqc resolves the default manifest path (`rules_templates/rules-all.toml`) relative to the *current working directory*, not the binary location. When running sqc from outside the sqc repo, pass `--manifest` with an absolute path, as shown in all commands below.

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
  --manifest ~/data/tools_sqc/rules_templates/rules-all.toml \
  --export ~/data/comparisons/results/sqc/libcrc/results.json
```

#### cppcheck

```bash
cppcheck \
  --enable=all \
  --std=c11 \
  --xml --xml-version=2 \
  --suppress=missingIncludeSystem \
  -I ~/data/comparisons/libcrc/include \
  ~/data/comparisons/libcrc/src/ \
  2> ~/data/comparisons/results/cppcheck/libcrc/results.xml
```

#### clang-tidy

libcrc uses a plain Makefile with no configure, so use `bear` to capture the compilation database, or scan files directly (include path is simple enough):

```bash
# Option A — bear (recommended, captures exact compiler flags)
# make clean is required — bear only captures invocations during an actual build;
# if the project is already built, make does nothing and compile_commands.json is empty.
cd ~/data/comparisons/libcrc && make clean && bear -- make
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
  --manifest ~/data/tools_sqc/rules_templates/rules-all.toml \
  --export ~/data/comparisons/results/sqc/sqlite/results.json
```

#### cppcheck

```bash
cppcheck \
  --enable=all \
  --std=c11 \
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
make clean && bear -- make -j$(nproc)
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
  --manifest ~/data/tools_sqc/rules_templates/rules-all.toml \
  --export ~/data/comparisons/results/sqc/mosquitto/results.json
```

#### cppcheck

```bash
cppcheck \
  --enable=all \
  --std=c11 \
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

mosquitto requires `libcjson-dev` (used for MQTT message JSON handling). Install it first:

```bash
sudo apt install -y libcjson-dev
```

CMake natively exports `compile_commands.json`:

```bash
cmake \
  -S ~/data/comparisons/mosquitto \
  -B ~/data/comparisons/mosquitto/build \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DWITH_TLS=OFF \
  -DWITH_WEBSOCKETS=OFF \
  -DWITH_TESTS=OFF

cmake --build ~/data/comparisons/mosquitto/build --clean-first -j$(nproc)

run-clang-tidy \
  -clang-tidy-binary clang-tidy \
  -checks='-*,cert-*,clang-analyzer-*' \
  -p ~/data/comparisons/mosquitto/build \
  2>&1 | tee ~/data/comparisons/results/clang-tidy/mosquitto/results.txt
```

Flags used:

| Flag | Purpose |
|------|---------|
| `-DWITH_TLS=OFF` | Skip OpenSSL/TLS support |
| `-DWITH_WEBSOCKETS=OFF` | Skip libwebsockets support |
| `-DWITH_TESTS=OFF` | Skip unit test build (avoids requiring GTest) |

---

### 8.4 curl

**Project**: `~/data/comparisons/curl`
**Build system**: CMake (preferred) or autoconf
**Source**: `lib/` (libcurl, ~200 `.c` files), `src/` (curl CLI)

#### sqc

```bash
~/data/tools_sqc/target/release/sqc ~/data/comparisons/curl \
  -d ~/data/comparisons/curl \
  --manifest ~/data/tools_sqc/rules_templates/rules-all.toml \
  --export ~/data/comparisons/results/sqc/curl/results.json
```

#### cppcheck

```bash
cppcheck \
  --enable=all \
  --std=c11 \
  --xml --xml-version=2 \
  --suppress=missingIncludeSystem \
  -I ~/data/comparisons/curl/include \
  -I ~/data/comparisons/curl/lib \
  ~/data/comparisons/curl/lib/ \
  ~/data/comparisons/curl/src/ \
  2> ~/data/comparisons/results/cppcheck/curl/results.xml
```

#### clang-tidy

curl 8.19+ requires `libpsl` unconditionally (the CMake option `CURL_DISABLE_LIBPSL` does **not** remove the hard dependency). Install it before configuring:

```bash
sudo apt install -y libpsl-dev
```

Configure and build:

```bash
cmake \
  -S ~/data/comparisons/curl \
  -B ~/data/comparisons/curl/build \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DBUILD_SHARED_LIBS=OFF \
  -DCURL_USE_OPENSSL=OFF \
  -DCURL_DISABLE_LDAP=ON \
  -DUSE_LIBIDN2=OFF \
  -DUSE_NGHTTP2=OFF \
  -DCURL_ZSTD=OFF

cmake --build ~/data/comparisons/curl/build --clean-first -j$(nproc)

run-clang-tidy \
  -clang-tidy-binary clang-tidy \
  -checks='-*,cert-*,clang-analyzer-*' \
  -p ~/data/comparisons/curl/build \
  2>&1 | tee ~/data/comparisons/results/clang-tidy/curl/results.txt
```

Flags used to avoid missing optional dependencies in a minimal environment:

| Flag | Purpose |
|------|---------|
| `-DCURL_USE_OPENSSL=OFF` | Skip OpenSSL (not installed) |
| `-DCURL_DISABLE_LDAP=ON` | Skip LDAP support |
| `-DUSE_LIBIDN2=OFF` | Skip libidn2 (internationalized domain names) |
| `-DUSE_NGHTTP2=OFF` | Skip HTTP/2 support |
| `-DCURL_ZSTD=OFF` | Skip zstd compression |

If the cmake configure step fails due to missing packages, delete the build directory and retry: `rm -rf ~/data/comparisons/curl/build`. A partial configure leaves state that causes misleading errors on re-runs.

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
  --manifest ~/data/tools_sqc/rules_templates/rules-all.toml \
  --export ~/data/comparisons/results/sqc/hostap/results.json
```

#### cppcheck

```bash
cppcheck \
  --enable=all \
  --std=c11 \
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
make clean 2>/dev/null; bear -- make -j$(nproc) wpa_supplicant 2>/dev/null || true

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

## 9. Verifying Results Are Valid

After running all three tools, verify that each output file is non-empty and contains actual findings — not just empty output, tool errors, or a check listing with no diagnostics. This section documents the verification steps and all pitfalls encountered during initial setup on Ubuntu 24.04 with libcrc.

---

### 9.1 Known Pitfalls and Fixes

These issues were each encountered and resolved during the first real run against libcrc. Check for all of them before trusting results.

#### sqc: manifest not found when run outside the repo

**Symptom**:
```
Error: Failed to read manifest file: rules_templates/rules-all.toml: No such file or directory
```

**Cause**: sqc resolves the default manifest path relative to the current working directory, not the binary location. Running sqc from a project directory (e.g., `~/data/comparisons/libcrc`) fails because `rules_templates/rules-all.toml` does not exist there.

**Fix**: Always pass `--manifest` with an absolute path:
```bash
~/data/tools_sqc/target/release/sqc ~/data/comparisons/libcrc \
  --manifest ~/data/tools_sqc/rules_templates/rules-all.toml \
  ...
```

---

#### cppcheck: `--addon=cert` not available on Ubuntu 24.04

**Symptom**:
```
Did not find addon cert.py
```

**Cause**: The Ubuntu 24.04 apt package of cppcheck 2.13 does not ship `cert.py`. Available addons are: `findcasts`, `misc`, `misra`, `naming`, `namingng`, `threadsafety`, `y2038`. In cppcheck 2.x, CERT C checks were folded into the built-in checker.

**Fix**: Remove `--addon=cert`. The `--enable=all` flag already activates the built-in CERT-related checks.

---

#### cppcheck: blank lines inside `\`-continued commands break the shell

**Symptom**:
```
cppcheck: error: no C or C++ source files found.
```
...when the command visually looks correct.

**Cause**: A blank line between two `\`-continued lines terminates the shell command at that point. The source path never reaches cppcheck. This can be silently introduced when editing the command (e.g., removing a flag line leaves an empty line behind).

**Fix**: Ensure there are no blank lines between any `\`-continued lines in cppcheck commands.

---

#### cppcheck: passing `-I /usr/include` causes parse errors in system headers

**Symptom**:
```
[information] toomanyconfigs: Too many #ifdef configurations — cppcheck only checks 12 of 38
[error] syntaxError: syntax error @ stdlib.h:980
```
Finding count drops to near zero.

**Cause**: cppcheck has its own internal model of standard library functions. Passing `-I /usr/include` causes it to actually parse GCC system headers, which contain compiler-specific extensions cppcheck cannot handle.

**Fix**: Never pass `-I /usr/include` to cppcheck. Use `--suppress=missingIncludeSystem` to silence the "include not found" information messages instead. cppcheck will still analyse the code correctly using its internal stdlib model.

---

#### clang-tidy: `compile_commands.json` is empty when project is already built

**Symptom**: `compile_commands.json` is 2–3 bytes (`[]`). clang-tidy output contains only the "Enabled checks:" listing with no diagnostics.

**Cause**: `bear` works by intercepting compiler invocations via `LD_PRELOAD`. If `make` finds all targets up-to-date, no compiler is invoked and no entries are recorded. The resulting `compile_commands.json` is empty.

**Fix**: Always run `make clean` (or `cmake --build ... --clean-first` for CMake projects) immediately before `bear -- make`. Verify the entry count before running clang-tidy:
```bash
python3 -c "import json; db=json.load(open('compile_commands.json')); print(len(db), 'entries')"
# Must be > 0
```

---

#### clang-tidy: output file looks non-empty but contains no diagnostics

**Symptom**: Results file is 5–7 KB but all content is the "Enabled checks:" block. Grepping for `warning:` or `error:` returns 0 matches.

**Cause**: `run-clang-tidy` always prints the enabled check list regardless of whether it found anything. This can mask the empty-`compile_commands.json` problem above.

**Fix**: After running clang-tidy, explicitly count diagnostic lines:
```bash
grep -c "warning:\|error:" ~/data/comparisons/results/clang-tidy/PROJECT/results.txt
# Must be > 0 for a project of any meaningful size
```

---

### 9.2 Verification Scripts

Run these immediately after generating results to confirm all three tools produced valid output.

#### Quick sanity check — all three tools

```bash
PROJECT=libcrc
RESULTS=~/data/comparisons/results

echo "=== sqc ==="
python3 -c "
import json, collections
data = json.load(open('$RESULTS/sqc/$PROJECT/results.json'))
sevs = collections.Counter(v['severity'] for v in data)
rules = collections.Counter(v['rule_id'] for v in data)
print(f'Total violations: {len(data)}')
print('By severity:', dict(sevs.most_common()))
print('Top 10 rules:', dict(rules.most_common(10)))
"

echo "=== cppcheck ==="
python3 -c "
import xml.etree.ElementTree as ET, collections
errors = ET.parse('$RESULTS/cppcheck/$PROJECT/results.xml').getroot().findall('.//error')
sevs = collections.Counter(e.get('severity') for e in errors)
ids  = collections.Counter(e.get('id') for e in errors)
cwes = collections.Counter(e.get('cwe') for e in errors if e.get('cwe'))
print(f'Total findings: {len(errors)}')
print('By severity:', dict(sevs.most_common()))
print('Top IDs:', dict(ids.most_common(10)))
print('CWEs hit:', dict(cwes.most_common(10)))
"

echo "=== clang-tidy ==="
python3 -c "
import re, collections
checks = collections.Counter()
files  = collections.Counter()
with open('$RESULTS/clang-tidy/$PROJECT/results.txt') as f:
    for line in f:
        m = re.match(r'^(\S[^:]+):\d+:\d+: (?:warning|error): .+\[(\S+)\]', line)
        if m:
            files[m.group(1).split('/')[-1]] += 1
            checks[m.group(2)] += 1
print(f'Total diagnostics: {sum(checks.values())}')
print('By check:', dict(checks.most_common(10)))
print('By file:', dict(files.most_common(10)))
"
```

Change `PROJECT=libcrc` to `sqlite`, `mosquitto`, `curl`, or `hostap` for other projects.

#### Validate compile_commands.json before running clang-tidy

```bash
python3 -c "
import json, sys
db = json.load(open('compile_commands.json'))
print(f'{len(db)} compilation units')
if len(db) == 0:
    print('ERROR: empty — run make clean && bear -- make first')
    sys.exit(1)
for e in db[:5]:
    print(' ', e['file'].split('/')[-1])
"
```

#### Check for cppcheck error conditions

```bash
# Flag any run that produced syntaxError or toomanyconfigs as primary output
python3 -c "
import xml.etree.ElementTree as ET
errors = ET.parse('results.xml').getroot().findall('.//error')
bad = [e for e in errors if e.get('id') in ('syntaxError', 'toomanyconfigs')]
real = [e for e in errors if e.get('id') not in ('syntaxError', 'toomanyconfigs', 'checkersReport', 'missingIncludeSystem')]
print(f'Real findings: {len(real)}, Problem indicators: {len(bad)}')
if bad:
    for e in bad[:3]:
        print(f\"  [{e.get('id')}] {e.get('msg','')[:80]}\")
"
```

---

### 9.3 libcrc Baseline Reference

libcrc was the first project validated end-to-end. These counts are the confirmed-good baseline.

**Environment**: sqc 0.2.3, cppcheck 2.7, clang-tidy 14 (Ubuntu 22.04, 10.0.0.63)

| Tool | Total findings | Breakdown |
|------|---------------|-----------|
| **sqc** | 954 violations | High: 407, Medium: 297, Low: 250 |
| **cppcheck** | 40 findings | style: 39 (`variableScope` 36, `unusedFunction` 2, `knownConditionTrueFalse` 1), information: 1 |
| **clang-tidy** | 52 diagnostics | `cert-err33-c`: 26, `clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling`: 24, `clang-diagnostic-error`: 2 |

**sqc top rules**: `EXP14-C` (106), `ERR33-C` (68), `EXP12-C` (62), `INT30-C` (60), `EXP19-C` (59)

**cppcheck CWEs**: CWE-398 (poor code quality, 36), CWE-561 (unused code, 2), CWE-571 (1)

**Interpretation**: cppcheck and clang-tidy find a small number of conservative, high-confidence issues. sqc finds orders of magnitude more by covering 283 CERT C rules rather than the ~20 checks the other tools implement for C. The disparity is expected and informative — it reflects rule coverage breadth, not false positive rate.

---

### 9.4 sqlite Baseline Reference

sqlite is the largest single-project benchmark (~354 C files across `src/`, `ext/`, `test/`, `autosetup/`). It uses extensive preprocessor conditionals, internal macros, and includes a 24K-line TCL interpreter (`jimsh0.c`). Expect high violation counts from all tools.

**Environment**: sqc 0.2.3, cppcheck 2.7, clang-tidy 14 (Ubuntu 22.04, 10.0.0.63)

| Tool | Total findings | By Severity | Top Rules/Checks |
|------|---------------|-------------|-----------------|
| **sqc** | **424,842** | Critical: 5,843 / High: 81,364 / Medium: 264,973 / Low: 72,662 | STR31-C (206,651), EXP34-C (41,885), DCL07-C (20,443), DCL31-C (16,038), API00-C (14,420) |
| **cppcheck** | **1,182** (993 real) | error: 37, style: 855, information: 210, warning: 67, portability: 13 | `variableScope` (505), `toomanyconfigs` (189), `unreadVariable` (103), `constParameter` (63), `unusedStructMember` (59) |
| **clang-tidy** | **2,291** | — | `cert-err33-c` (1,025), `DeprecatedOrUnsafeBufferHandling` (453), `clang-diagnostic-error` (405), `cert-str34-c` (124), `cert-err34-c` (76) |

**sqc KNOWN BUG — STR31-C `detect_manual_string_loop` runaway**: 206,431 of the 206,651 STR31-C violations are "Manual string copying loop without apparent bounds checking" — a single pattern accounting for **49% of all violations**. Root cause: the function's final fallback (`str31_c.rs:799-812`) searches the **entire source file** for any line containing `memcpy` + `strlen`/`string`. One matching line anywhere in a file causes the function to return `true` for every `while`/`for` node visited. `jimsh0.c` alone produces 180,297 violations from this bug. See §9.7 for details.

**cppcheck notes**: 189 `toomanyconfigs` entries (expected for sqlite's `#ifdef` complexity). Key real findings: `invalidPrintfArgType_sint` (CWE-686, 27), `objectIndex` (CWE-758, 21), `knownConditionTrueFalse` (CWE-570/571, 37).

**clang-tidy notes**: 405 `clang-diagnostic-error` from missing headers when scanning without `compile_commands.json`. `cert-err33-c` (1,025) dominates real diagnostics — unchecked return values throughout test harness and utility code.

---

### 9.5 curl Baseline Reference

curl is ~10× larger than libcrc (~220 C files in `lib/` + `src/`), with heavy use of preprocessor guards, function pointers, and platform abstraction macros.

**Environment**: sqc 0.2.3, cppcheck 2.7, clang-tidy 14 (Ubuntu 22.04, 10.0.0.63)

| Tool | Total findings | By Severity | Top Rules/Checks |
|------|---------------|-------------|-----------------|
| **sqc** | **207,476** | Critical: 2,276 / High: 40,260 / Medium: 118,836 / Low: 46,104 | STR31-C (93,140), EXP34-C (21,893), DCL07-C (10,725), DCL31-C (10,614), EXP19-C (9,135) |
| **cppcheck** | **551** (298 real) | error: 13, style: 213, information: 289, warning: 32, portability: 4 | `toomanyconfigs` (253), `variableScope` (95), `unreadVariable` (42), `ConfigurationNotChecked` (33), `knownConditionTrueFalse` (21) |
| **clang-tidy** | **1,653** | — | `clang-diagnostic-error` (1,024), `cert-err33-c` (366), `DeprecatedOrUnsafeBufferHandling` (211), `cert-err34-c` (20), `cert-str34-c` (6) |

**sqc notes**: STR31-C (93,140) accounts for 45% of all violations — same `detect_manual_string_loop` runaway bug as sqlite (§9.7). Excluding STR31-C, top rules are EXP34-C (21,893), DCL07-C (10,725), DCL31-C (10,614).

**cppcheck notes**: 253 `toomanyconfigs` + 33 `ConfigurationNotChecked` = 286 informational. Key real findings: `nullPointerRedundantCheck` (CWE-476, 20), `knownConditionTrueFalse` (21). Real actionable: 298.

**clang-tidy notes**: 1,024 `clang-diagnostic-error` from missing headers (no `compile_commands.json` used — direct file scan). Real diagnostics: `cert-err33-c` (366), `DeprecatedOrUnsafeBufferHandling` (211).

---

### 9.6 mosquitto Baseline Reference

mosquitto is a mid-size MQTT broker + client library (~121 C files in `lib/` + `src/`). Heavy use of compile-time feature flags means cppcheck enumerates many configurations per file.

**Environment**: sqc 0.2.3, cppcheck 2.7, clang-tidy 14 (Ubuntu 22.04, 10.0.0.63)

| Tool | Total findings | By Severity | Top Rules/Checks |
|------|---------------|-------------|-----------------|
| **sqc** | **47,417** | Critical: 1,181 / High: 18,540 / Medium: 11,790 / Low: 15,906 | EXP34-C (7,631), API00-C (3,081), DCL31-C (2,979), DCL07-C (2,975), MEM31-C (2,874) |
| **cppcheck** | **598** (458 real) | error: 51, style: 380, information: 143, warning: 24 | `variableScope` (236), `toomanyconfigs` (140), `noExplicitConstructor` (60), `uninitvar` (50), `constParameter` (19) |
| **clang-tidy** | **907** | — | `cert-err33-c` (477), `clang-diagnostic-error` (255), `cert-err34-c` (111), `DeprecatedOrUnsafeBufferHandling` (29) |

**sqc notes**: mosquitto is the one project where STR31-C is NOT the top rule — EXP34-C (7,631) dominates instead. STR31-C violations are modest here, suggesting the `detect_manual_string_loop` pattern fires less on mosquitto's coding style.

**cppcheck notes**: 50 `uninitvar` (CWE-457, error severity) are the most significant findings — potentially real uninitialized variable bugs in the broker code. Real actionable: 458.

**clang-tidy notes**: `cert-err33-c` (477) dominates — unchecked return values. `cert-err34-c` (111) flags `atoi()` usage in plugin configuration parsers. 255 `clang-diagnostic-error` from missing headers.

---

### 9.7 hostap Baseline Reference

hostap (wpa_supplicant/hostapd) is a large networking project with shared source in `src/` plus application code in `wpa_supplicant/` and `hostapd/`.

**Environment**: sqc 0.2.3, cppcheck 2.7, clang-tidy 14 (Ubuntu 22.04, 10.0.0.63)

| Tool | Total findings | By Severity | Top Rules/Checks |
|------|---------------|-------------|-----------------|
| **sqc** | **473,862** | Critical: 7,247 / High: 119,854 / Medium: 235,290 / Low: 111,471 | STR31-C (170,586), EXP34-C (69,164), DCL08-C (25,296), EXP19-C (25,140), API00-C (19,832) |
| **cppcheck** | **1,066** (813 real) | error: 126, style: 663, information: 255, warning: 16, portability: 4, performance: 2 | `variableScope` (390), `toomanyconfigs` (253), `uninitvar` (89), `constParameter` (86), `knownConditionTrueFalse` (54) |
| **clang-tidy** | **1,083** | — | `clang-diagnostic-error` (517), `cert-err34-c` (377), `cert-dcl37-c` (66), `cert-err33-c` (50), `cert-str34-c` (10) |

**sqc notes**: STR31-C (170,586) accounts for 36% of all violations — same runaway bug (§9.8). Excluding STR31-C, top rules are EXP34-C (69,164), DCL08-C (25,296), EXP19-C (25,140).

**cppcheck notes**: 89 `uninitvar` (CWE-457) at error severity — the highest count of any project, likely real bugs in network protocol handlers. 253 `toomanyconfigs` from `#ifdef` complexity. Real actionable: 813.

**clang-tidy notes**: hostap is the only project where `cert-err34-c` (377) dominates over `cert-err33-c` (50) — heavy `atoi()`/`strtol()` usage in configuration parsing. 517 `clang-diagnostic-error` from missing headers.

---

### 9.8 STR31-C `detect_manual_string_loop` Bug (FIXED)

**Severity**: High — caused 36–49% of all sqc violations on 3 of 5 real-world projects.

**Root cause** (`str31_c.rs:799-812`): The `detect_manual_string_loop()` function had a final fallback that iterated ALL lines in the source file looking for any line containing both `memcpy` and (`strlen` or `string`). If found, it returned `true` for the **current node regardless of context**. Since this function was called on every `while_statement` and `for_statement` node, one matching line anywhere in a file caused every loop in the file to generate a violation.

**Impact by project (before fix)**:

| Project | STR31-C violations | % of total | Worst file |
|---------|-------------------|------------|------------|
| sqlite | 206,651 | 49% | `jimsh0.c` (180,297) |
| hostap | 170,586 | 36% | spread across `src/` |
| curl | 93,140 | 45% | spread across `lib/` |
| mosquitto | low | — | not triggered |
| libcrc | low | — | not triggered |

**Fix applied** (2026-02-25): Rewrote `detect_manual_string_loop` with three changes:
1. **Deleted the file-wide memcpy+strlen fallback** — the root cause of the runaway
2. **Condition-only matching**: checks AST `condition` field, not full loop text. Requires null-terminator walk (`!= '\0'`, `!= 0` with dereference) or `getchar` in the condition.
3. **Body-only write detection**: requires specific write patterns (`*ptr++ =`, `dest[i] = src[i]`) in the loop body, not just any `++`.
4. **Improved `is_string_memcpy`**: added `strlen()` as size argument detection — `memcpy(dest, src, strlen(src))` without `+ 1` is now caught regardless of variable names.

**Verification**: `jimsh0.c` STR31-C dropped from 180,297 to 10 (all legitimate sub-checks). 2781/2781 tests pass. Zero Juliet TP impact (Juliet uses standard library functions, not hand-written loops).

---

## 10. Notes on Methodology

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

---

## 12. TODO: Additional Tools to Evaluate

> **Status**: Research task for next session. The goal is to identify open-source or freely-available static analysis tools with meaningful CERT C / CWE coverage that can be added to the distributed benchmark alongside cppcheck and clang-tidy.

### Candidates to Investigate

| Tool | Type | Notes |
|------|------|-------|
| **Infer** (Meta) | Open source, flow-sensitive | Already in §7 comparison table. Handles null deref and memory safety well. Slower than pattern-based tools. |
| **Frama-C** (CEA) | Open source, formal methods | Eva (value analysis) and WP (deductive verification) plugins cover many CERT C rules. Very thorough but slow; may need dedicated node. |
| **Semgrep CE** | Open source, pattern-based | Already in §7 comparison table. Fast, easy to extend with custom CERT C patterns. |
| **Flawfinder** | Open source, pattern-based | Lightweight, focuses on CWE security patterns (buffer overflows, format strings). Quick to run. |
| **PVS-Studio** | Commercial (free for OSS) | Has explicit CERT C mapping in their documentation. Worth checking if free tier covers the projects we benchmark. |
| **Coverity Scan** | Commercial (free for OSS) | Synopsys; defect density data only, no public CERT C mapping — but widely cited. Limited API access for automation. |
| **CodeChecker** | Open source | Frontend/aggregator for Clang Static Analyzer results with a web UI and diff capability. Not a new analysis engine but may simplify result management. |

### Research Questions for Each Tool

- Does it have explicit CERT C rule mapping, or only CWE/generic defect categories?
- Can it run headlessly from a shell command suitable for GNU Parallel distribution?
- What is the approximate runtime on libcrc (baseline: cppcheck=~5s, clang-tidy=~10s, sqc=~2s)?
- Does it produce machine-readable output (XML, JSON, SARIF) for scripted comparison?
- Are there known Juliet benchmark results published for this tool?

### Priority

Infer and Frama-C are highest priority — both are flow-sensitive (unlike cppcheck/clang-tidy) and would provide the most meaningful comparison to sqc's analysis depth. Semgrep CE is useful for establishing a pattern-matching baseline. Flawfinder is a quick win if runtime is negligible.


---

## 11. Distributed Benchmarking with GNU Parallel (8-Node Cluster)

The primary use case here is fast re-benchmarking: after a rule change, rebuild sqc and get updated TP/FP numbers across Juliet and all real-world projects as quickly as possible. GNU Parallel over SSH is the right tool for this — it extends the same `xargs -P` pattern already used throughout this doc to span multiple machines, with no daemon infrastructure.

**Key principle**: cppcheck and clang-tidy results are stable across sqc rule changes — run them once and cache. Only sqc needs to re-run after each rule iteration. The fast re-benchmark path is sqc-only.

---

### 11.1 Prerequisites

**Install GNU Parallel** on all nodes:

```bash
sudo apt install -y parallel
# Silence the citation notice
parallel --citation <<< "will cite" 2>/dev/null || true
```

**SSH key auth** from head node to all compute nodes (no password prompts):

```bash
ssh-keygen -t ed25519 -f ~/.ssh/id_benchmark -N ""
for node in node1 node2 node3 node4 node5 node6 node7 node8; do
  ssh-copy-id -i ~/.ssh/id_benchmark.pub $node
done
```

**Shared filesystem**: NFS or any shared mount at a consistent path across all nodes is strongly preferred. If unavailable, see the rsync pattern at the end of §11.3.

**Environment variables** (add to `~/.bashrc` on the head node):

```bash
export SQC_BIN=/home/brandon/data/tools_sqc/target/release/sqc
export SQC_MANIFEST=/home/brandon/data/tools_sqc/rules_templates/rules-all.toml
export JULIET_DIR=/shared/data/juliet/testcases
export JULIET_SUPPORT=/shared/data/juliet/testcasesupport
export RESULTS_DIR=/shared/data/comparisons/results
export NODES_FILE=/home/brandon/.benchmark_nodes
```

Create output directories once:

```bash
mkdir -p $RESULTS_DIR/{sqc,cppcheck,clang-tidy}/{juliet,libcrc,sqlite,mosquitto,curl,hostap}
mkdir -p logs/
```

---

### 11.2 Node File

GNU Parallel reads a node file where each line is `user@host/N` — the `/N` controls how many concurrent jobs run on that host. Set N to the number of cores you want to dedicate per node.

```
# ~/.benchmark_nodes
brandon@node1/8
brandon@node2/8
brandon@node3/8
brandon@node4/8
brandon@node5/8
brandon@node6/8
brandon@node7/8
brandon@node8/8
```

Include the head node with `:/8` if you want it to also do work (it runs the coordinator process but is otherwise idle during distribution).

Verify connectivity before the first real run:

```bash
parallel --sshloginfile $NODES_FILE \
  --nonall \
  'echo "$(hostname): $(nproc) cores, $(free -h | awk "/^Mem/{print $2}") RAM"'
```

---

### 11.3 Fast Re-Benchmark Workflow

This is the primary use case: a rule has been changed, sqc rebuilt, and you want updated TP/FP numbers as fast as possible.

**Step 1 — Rebuild** (on head node, shared FS makes the binary immediately available everywhere):

```bash
cd /home/brandon/data/tools_sqc
cargo build --release 2>&1 | tail -3
```

If no shared FS, push the binary to all nodes after building:

```bash
parallel --sshloginfile $NODES_FILE \
  --nonall \
  "rsync -az $SQC_BIN {}/sqc_bin"
# then use {}/sqc_bin instead of $SQC_BIN in job commands
```

**Step 2 — Run sqc across Juliet + real-world projects in parallel**:

```bash
# Generate CWE list
find $JULIET_DIR -maxdepth 1 -type d -name 'CWE*' | sort > /tmp/cwe_dirs.txt

# Juliet: one job per CWE directory, distributed across all nodes
parallel \
  --sshloginfile $NODES_FILE \
  --jobs 4 \
  --eta \
  '$SQC_BIN {} \
    -d $JULIET_DIR \
    -d $JULIET_SUPPORT \
    --manifest $SQC_MANIFEST \
    --export $RESULTS_DIR/sqc/juliet/$(basename {}).csv \
    2>/dev/null' \
  :::: /tmp/cwe_dirs.txt

# Real-world projects: one job per project, run concurrently
parallel \
  --sshloginfile $NODES_FILE \
  --jobs 1 \
  '$SQC_BIN $RESULTS_DIR/../{} \
    -d $RESULTS_DIR/../{} \
    --manifest $SQC_MANIFEST \
    --export $RESULTS_DIR/sqc/{}/results.csv \
    2>/dev/null' \
  ::: libcrc sqlite mosquitto curl hostap
```

**Step 3 — Aggregate**:

```bash
# Merge Juliet CSVs and run TP/FP analysis
MERGED=$RESULTS_DIR/sqc/juliet/all_cwe.csv
head -1 $(ls $RESULTS_DIR/sqc/juliet/CWE*.csv | head -1) > "$MERGED"
for f in $RESULTS_DIR/sqc/juliet/CWE*.csv; do tail -n +2 "$f"; done >> "$MERGED"

python3 /home/brandon/data/tools_sqc/scripts/analyze_juliet_results.py \
  --csv "$MERGED" \
  --dir "$JULIET_DIR" \
  | tee $RESULTS_DIR/sqc/juliet/tp_fp_summary.txt
```

Total turnaround for steps 2–3 with 8 nodes at 8 jobs/node: Juliet (~130 CWEs) in roughly 2–4 minutes; all 5 real-world projects in parallel on separate nodes simultaneously.

---

### 11.4 Juliet Benchmark Distribution

The Juliet benchmark distributes naturally — each CWE directory is an independent unit of work.

```bash
# Full three-tool Juliet run (cppcheck and clang-tidy results can be cached;
# only re-run sqc between rule iterations)

# sqc — fast, re-run every iteration
parallel \
  --sshloginfile $NODES_FILE \
  --jobs 4 \
  --joblog logs/juliet_sqc.log \
  --resume-failed \
  '$SQC_BIN {1} \
    -d $JULIET_DIR -d $JULIET_SUPPORT \
    --manifest $SQC_MANIFEST \
    --export $RESULTS_DIR/sqc/juliet/$(basename {1}).csv \
    2>/dev/null' \
  :::: /tmp/cwe_dirs.txt

# cppcheck — run once, results stable across sqc rule changes
parallel \
  --sshloginfile $NODES_FILE \
  --jobs 4 \
  --joblog logs/juliet_cppcheck.log \
  --resume-failed \
  'cppcheck \
    --enable=all --std=c11 --xml --xml-version=2 \
    --suppress=missingIncludeSystem \
    {1} \
    2> $RESULTS_DIR/cppcheck/juliet/$(basename {1}).xml' \
  :::: /tmp/cwe_dirs.txt

# clang-tidy — run once, results stable across sqc rule changes
parallel \
  --sshloginfile $NODES_FILE \
  --jobs 2 \
  --joblog logs/juliet_clang_tidy.log \
  --resume-failed \
  'find {1} -name "*.c" | \
    xargs -P 4 -I{f} clang-tidy \
      -checks="-*,cert-*,clang-analyzer-*" {f} \
      -- -std=c11 -I $JULIET_SUPPORT \
    > $RESULTS_DIR/clang-tidy/juliet/$(basename {1}).txt 2>&1' \
  :::: /tmp/cwe_dirs.txt
```

`--resume-failed` re-runs only failed jobs on the next invocation (uses `--joblog`). Useful when a node goes down mid-run.

---

### 11.5 Real-World Project Distribution

Real-world projects vary in size. Run them in parallel across nodes, assigning heavier projects to dedicated nodes:

```bash
# Parallel sqc across all projects simultaneously
# Each project runs on whichever node is free
parallel \
  --sshloginfile $NODES_FILE \
  --jobs 1 \
  --colsep '\t' \
  '$SQC_BIN {2} \
    -d {2} \
    --manifest $SQC_MANIFEST \
    --export $RESULTS_DIR/sqc/{1}/results.csv \
    2>/dev/null && echo "Done: {1}"' \
  :::: <(printf "libcrc\t/shared/data/comparisons/libcrc\n\
sqlite\t/shared/data/comparisons/sqlite\n\
mosquitto\t/shared/data/comparisons/mosquitto\n\
curl\t/shared/data/comparisons/curl\n\
hostap\t/shared/data/comparisons/hostap\n")
```

To run cppcheck across all projects in parallel (one-time baseline, stable results):

```bash
declare -A CPP_SRC CPP_INC
CPP_SRC[libcrc]="/shared/data/comparisons/libcrc/src/"
CPP_INC[libcrc]="-I /shared/data/comparisons/libcrc/include"
CPP_SRC[sqlite]="/shared/data/comparisons/sqlite/src/"
CPP_INC[sqlite]="-I /shared/data/comparisons/sqlite/src"
CPP_SRC[mosquitto]="/shared/data/comparisons/mosquitto/lib/ /shared/data/comparisons/mosquitto/src/"
CPP_INC[mosquitto]="-I /shared/data/comparisons/mosquitto/include -i /shared/data/comparisons/mosquitto/deps"
CPP_SRC[curl]="/shared/data/comparisons/curl/lib/ /shared/data/comparisons/curl/src/"
CPP_INC[curl]="-I /shared/data/comparisons/curl/include -I /shared/data/comparisons/curl/lib"
CPP_SRC[hostap]="/shared/data/comparisons/hostap/src/ /shared/data/comparisons/hostap/wpa_supplicant/"
CPP_INC[hostap]="-I /shared/data/comparisons/hostap/src -I /shared/data/comparisons/hostap/src/utils"

for PROJECT in libcrc sqlite mosquitto curl hostap; do
  echo "$PROJECT	${CPP_SRC[$PROJECT]}	${CPP_INC[$PROJECT]}"
done | \
parallel \
  --sshloginfile $NODES_FILE \
  --jobs 1 \
  --colsep '\t' \
  'cppcheck --enable=all --std=c11 --xml --xml-version=2 \
    --suppress=missingIncludeSystem {3} {2} \
    2> $RESULTS_DIR/cppcheck/{1}/results.xml && echo "Done cppcheck: {1}"'
```

---

### 11.6 Scaling to More Projects

Adding a new real-world project to the benchmark requires only:

1. Clone the project to `/shared/data/comparisons/<project>/`
2. Add a row to the project list used in §11.5
3. Add cppcheck source/include entries to the `CPP_SRC`/`CPP_INC` maps
4. Run cppcheck once to establish a baseline (stable across sqc iterations)
5. The fast re-benchmark script (§11.3) picks it up automatically

The per-project cppcheck and clang-tidy baselines only need to be re-run if the project source is updated or a new tool version is being evaluated, not between sqc rule iterations.

---

## 13. Operational Notes from First Run (Ubuntu 22.04, 2026-02-24)

This section records lessons learned from the first full comparison run on a fresh Ubuntu 22.04
machine. Use this as a checklist to avoid repeating the same mistakes.

### 13.1 Setup Prerequisites

**Build dependencies** — sqc fails to build without these:

```bash
sudo apt install -y pkg-config libssl-dev
```

`libssl3` (runtime library) is installed by default on Ubuntu 22.04, but `libssl-dev` (headers)
and `pkg-config` are not. Without them, `cargo build --release` fails with:
```
error: could not find `pkg-config` / could not find OpenSSL installation
```

**cppcheck version** — Ubuntu 22.04 apt ships cppcheck **2.7** (not 2.13 as documented for 24.04).
This version does not support `--output-format=sarif` or the MISRA addon. All other flags
documented in this file work correctly with 2.7.

**clang-tidy version** — Ubuntu 22.04 ships LLVM 21.x via the LLVM apt repository at
`~/.local/bin/clang-tidy` (version 21.1.6). The apt package from universe is older (14.x).
Both work for CERT C checks; use whichever is already installed.

### 13.2 Project Paths

Projects are cloned to `~/data/<project>/` (not `~/data/comparisons/<project>/` as documented).
Adjust all commands accordingly:

```bash
# Document says: ~/data/comparisons/libcrc
# Actual path:   ~/data/libcrc
```

Output results are written to `~/data/results/{sqc,cppcheck,clang-tidy}/<project>/results.*`.

Create output directories once:

```bash
mkdir -p ~/data/results/{sqc,cppcheck,clang-tidy}/{libcrc,sqlite,mosquitto,curl,hostap}
```

### 13.3 cppcheck: CRITICAL — Use `-j N` and `--max-configs=3`

**Without these flags, cppcheck is completely impractical on large projects:**

| Project | Files | Single-threaded runtime | `-j 8 --max-configs=3` runtime |
|---------|-------|------------------------|-------------------------------|
| libcrc  | 16    | ~5 min                 | ~1 min                        |
| sqlite  | 312   | **3+ hours** (killed at 17%)| ~30 min               |
| mosquitto | 121 | ~20 min               | ~5 min                        |
| curl    | ~220  | (not measured)         | ~15 min (estimated)           |
| hostap  | 504   | (not measured)         | ~30 min (estimated)           |

The root cause is that SQLite uses massive `#ifdef` configuration enumeration. Without
`--max-configs=3`, cppcheck checks **~15 configurations per file** by default for SQLite, making
`jimsh0.c` alone (24K lines, a Tcl interpreter embedded in the build system) take 164+ CPU minutes.

**Always run cppcheck with:**

```bash
cppcheck \
  --enable=all \
  --std=c11 \
  --xml --xml-version=2 \
  --suppress=missingIncludeSystem \
  --max-configs=3 \
  -j $(nproc) \
  ~/data/$proj/
```

**Note**: `-j N` disables the `unusedFunction` check (cppcheck limitation). This is acceptable —
the important checks (memory, null pointer, CERT C) are unaffected.

**Stuck worker workaround**: Even with `--max-configs=3 -j 8`, cppcheck occasionally spawns a
worker that hangs indefinitely on a single file (e.g., `jimsh0.c` in sqlite — a 24K-line Tcl
interpreter embedded in the autoconf build system). Detect and kill the stuck worker with:

```bash
# Run this in a separate terminal while cppcheck is running on large projects
watch -n 30 'ps aux | grep cppcheck | grep -v grep | awk "{print \$2, \"time:\", \$10, \"cpu:\", \$3}"'

# If a worker has been running for 20+ minutes with 99%+ CPU, kill it:
kill <PID>
# The main cppcheck process (which shows 0% CPU while waiting) will
# finalize results collected so far and move to the next project.
```

This pattern was observed on sqlite (jimsh0.c) and hostap. Killing the stuck worker
results in partial analysis of the affected file — all other files are unaffected.

### 13.4 sqc: Run Times (Single-Threaded, Release Build)

sqc is single-threaded per invocation but individual project runs can be backgrounded:

| Project | Files | Violations | Runtime |
|---------|-------|-----------|---------|
| libcrc  | 16    | 954       | ~2 min  |
| sqlite  | 312   | 424,842   | ~78 min |
| mosquitto | 121 | 47,417   | ~24 min |
| curl    | ~220  | 207,476   | ~17 min |
| hostap  | 504   | 473,862   | ~84 min |

sqc is sequential in the comparison loop. Total wall-clock time: ~3.5 hours for all 5 projects.

### 13.5 clang-tidy: Run Times

clang-tidy with `-P $(nproc)` direct file scan (no compile_commands.json) completes all 5
projects in ~30 minutes total. It is the fastest of the three tools.

### 13.6 STR31-C Regression Detected (feature/exp34c-deref-after-check branch)

**Important finding**: The current sqc branch shows a severe STR31-C false-positive explosion
on large projects. This did NOT appear in the baseline results (sqc Round 9 / v0.2.3 and earlier).

| Project | STR31-C findings | Expected |
|---------|-----------------|----------|
| curl    | 93,140          | Not in baseline top-10 |
| sqlite  | 206,651         | Not in baseline top-10 |
| hostap  | 170,586         | Not in baseline top-10 |

Root cause: **36,085 identical** "Manual string copying loop without apparent bounds checking
detected" messages appear in `curl/lib/vtls/openssl.c` (5,479 lines). This is ~6.5 hits per
line — physically impossible for genuine findings. The STR31-C rule is triggering on every
iteration of string-copy loops rather than once per loop.

The total violation count for curl jumped from **131,445 (baseline) to 207,476 (+58%)** due
entirely to this regression. Other rules (EXP34-C, DCL07-C, DCL31-C) are broadly consistent
with baseline.

**Action**: Investigate STR31-C loop detection logic before the next benchmark run. The rule
should fire once per loop, not once per iteration.

### 13.7 Baseline vs Current Branch Comparison

| Project | Baseline | Current (feature branch) | Delta | Note |
|---------|----------|--------------------------|-------|------|
| libcrc  | 1,109    | 954                      | −14%  | Normal variation |
| mosquitto | 59,176 | 47,417                  | −20%  | Improvement (fewer FPs) |
| curl    | 131,445  | 207,476                  | +58%  | **STR31-C regression** |
| sqlite  | N/A      | 424,842                  | —     | First run |
| hostap  | N/A      | 473,862                  | —     | First run |

The −20% reduction in mosquitto is likely a genuine improvement from DCL31-C and DCL07-C
being tuned (those rules dropped from ~6,820 to ~2,975 each vs baseline).

### 13.7b Full Results Table (2026-02-24, feature/exp34c-deref-after-check)

| Project | sqc violations | cppcheck findings | clang-tidy diagnostics |
|---------|---------------|-------------------|----------------------|
| libcrc  | 954           | 40                | 52                   |
| sqlite  | 424,842       | 1,182             | 2,291                |
| mosquitto | 47,417      | 598               | 907                  |
| curl    | 207,476       | 551               | 1,653                |
| hostap  | 473,862       | 1,066             | 1,083                |

**sqc top rules by project:**
- libcrc: EXP14-C (106), ERR33-C (68), EXP12-C (62), INT30-C (60), EXP19-C (59)
- sqlite: STR31-C (206,651 — **regression**), EXP34-C (41,885), DCL07-C (20,443)
- mosquitto: EXP34-C (7,631), API00-C (3,081), DCL31-C (2,979), DCL07-C (2,975)
- curl: STR31-C (93,140 — **regression**), EXP34-C (21,893), DCL07-C (10,725)
- hostap: STR31-C (170,586 — **regression**), EXP34-C (69,164), DCL08-C (25,296)

**cppcheck top findings by project (excluding information/toomanyconfigs):**
- libcrc: variableScope (36), unusedFunction (2)
- sqlite: variableScope (505), unreadVariable (103), constParameter (63)
- mosquitto: variableScope (236), uninitvar (50 — real bugs), constParameter (19)
- curl: variableScope (95), unreadVariable (42), knownConditionTrueFalse (21)
- hostap: variableScope (390), uninitvar (89 — real bugs), constParameter (86)

**clang-tidy note**: `clang-diagnostic-error` counts (405 in sqlite, 1024 in curl, 517 in
hostap) represent files that failed to parse (missing headers, cross-TU dependencies). These
are not CERT C findings — they indicate the direct file scan approach (without
compile_commands.json) is incomplete for complex projects. For accurate clang-tidy results on
curl/sqlite/hostap, use `bear -- make` to capture compile_commands.json first.

### 13.8 Fast Re-Run After Bug Fixes

After fixing STR31-C (or any rule), re-run sqc only (cppcheck/clang-tidy don't change):

```bash
SQC=~/data/tools_sqc/target/release/sqc
MANIFEST=~/data/tools_sqc/rules_templates/rules-all.toml

cargo build --release -q  # rebuild after fix
for proj in libcrc sqlite mosquitto curl hostap; do
  $SQC ~/data/$proj \
    -d ~/data/$proj \
    --manifest $MANIFEST \
    --export ~/data/results/sqc/$proj/results.json \
    2>&1 | tail -1 &
done
wait
echo "All done"
```

This runs all 5 projects in parallel — total wall time drops to ~84 min (hostap, the longest).

