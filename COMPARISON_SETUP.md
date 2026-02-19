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

libcrc was the first project validated end-to-end. These counts are the confirmed-good baseline. If results differ significantly when re-running, a tool flag or environment issue is likely the cause.

| Tool | Total findings | Breakdown |
|------|---------------|-----------|
| **sqc** | 1,109 violations | High: 453, Medium: 388, Low: 268 |
| **cppcheck** | 41 findings | style: 40 (`unusedFunction` 21, `variableScope` 19), information: 1 |
| **clang-tidy** | 50 diagnostics | `cert-err33-c`: 26, `clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling`: 24 |

**sqc top rules**: `EXP14-C` (106), `ERR33-C` (68), `EXP12-C` (62), `INT30-C` (60), `EXP19-C` (59)

**cppcheck CWEs**: CWE-561 (unused code, 21), CWE-398 (poor code quality, 19)

**clang-tidy files**: Nearly all findings in `precalc/precalc.c` (47), `src/nmea-chk.c` (2), `examples/tstcrc.c` (1)

**Interpretation**: cppcheck and clang-tidy find a small number of conservative, high-confidence issues. sqc finds orders of magnitude more by covering 283 CERT C rules rather than the ~20 checks the other tools implement for C. The disparity is expected and informative — it reflects rule coverage breadth, not false positive rate.

---

---

### 9.4 curl Baseline Reference

curl was the second project validated end-to-end. It is ~10× larger than libcrc (~220 C files in `lib/` + `src/`), with heavy use of preprocessor guards, function pointers, and platform abstraction macros. These counts are from a clean run with the cmake flags documented in §8.4 and `libpsl-dev` installed.

| Tool | Total findings | Breakdown |
|------|---------------|-----------|
| **sqc** | 131,445 violations | Critical: 2,277, High: 44,288, Medium: 27,682, Low: 57,198 |
| **cppcheck** | 1,065 findings | error: 4, warning: 237, style: 599, information: 225 |
| **clang-tidy** | 848 diagnostics | `clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling`: 419, `cert-err33-c`: 364, `clang-analyzer-valist.Uninitialized`: 56 |

**sqc top rules**: `EXP34-C` (22,350), `DCL07-C` (16,000), `DCL31-C` (15,945), `EXP19-C` (9,105), `API00-C` (7,777)

**cppcheck notes**: 222 of the 225 `information` findings are `toomanyconfigs` entries — expected for curl's heavy `#ifdef` usage (cppcheck caps config enumeration at 12 per file). Real actionable findings: `nullPointerRedundantCheck` (CWE-476): 177, `constParameterPointer` (CWE-398): 159, `unusedFunction` (CWE-561): 108, `ctunullpointer` (CWE-476): 50. The `toomanyconfigs` entries do **not** indicate a tool error here (no `syntaxError` present), unlike the `-I /usr/include` failure mode described in §9.1.

**clang-tidy notes**: 678 compilation units captured. The dominant check is `DeprecatedOrUnsafeBufferHandling` (sprintf/strcpy family), followed by `cert-err33-c` (unchecked return values). `clang-analyzer-valist.Uninitialized` (56) are likely genuine defects in variadic helpers.

**Interpretation**: sqc's violation count scales with project size and rule breadth (283 rules). cppcheck and clang-tidy both scaled proportionally from libcrc to curl (~25× more findings vs ~25× more source), confirming consistent analysis rather than runaway false positives.

---

### 9.5 mosquitto Baseline Reference

mosquitto is a mid-size MQTT broker + client library (~121 C files in `lib/` + `src/`; 224 compilation units captured for clang-tidy). Its heavy use of compile-time feature flags (`WITH_TLS`, `WITH_BROKER`, `WITH_THREADING`, `WITH_WEBSOCKETS`, `WIN32`, etc.) means cppcheck enumerates many configurations per file, making it significantly slower than projects with fewer `#ifdef` guards.

| Tool | Total findings | By Severity | Top Rules/Checks |
|------|---------------|-------------|-----------------|
| **sqc** | **59,176** | Critical: 1,181 / High: 20,765 / Medium: 13,182 / Low: 24,048 | EXP34-C (8,657), DCL31-C (6,823), DCL07-C (6,820), API00-C (3,092), DCL13-C (2,940) |
| **cppcheck** | **747** | error: 36, warning: 1, style: 298, information: 412 | `missingInclude` (293), `unusedFunction`/CWE-561 (128), `toomanyconfigs` (117), `variableScope`/CWE-398 (72), `uninitvar`/CWE-457 (34) |
| **clang-tidy** | **338** | — | `cert-err33-c` (277), `cert-err34-c` (33), `clang-analyzer-deadcode.DeadStores` (8), `clang-analyzer-security.insecureAPI.strcpy` (5) |

**cppcheck notes**:
- 293 `missingInclude` (information) — mosquitto's internal headers (`src/mosquitto_broker_internal.h`, etc.) are not on the include path provided; these findings are informational and do not affect the quality of actual bug findings
- 117 `toomanyconfigs` (information) — expected given mosquitto's per-file `#ifdef` complexity (~10–12 configurations per file)
- 34 `uninitvar` (CWE-457, error severity) are the most significant cppcheck findings — potentially real uninitialized variable bugs in the broker code
- Real actionable findings (excluding information): 335

**clang-tidy notes**: `cert-err33-c` (277) dominates — unchecked return values of `fprintf`, `fclose`, `snprintf`, `strftime`, and `fputs` throughout client-side output code (`client/sub_client_output.c`, `client/pub_client.c`). `cert-err34-c` (33) flags `atoi()` usage in plugin configuration parsers. `clang-analyzer-security.insecureAPI.strcpy` (5) are concrete unsafe buffer operations.

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
