# SqC — Benchmark Installation & Setup

**Last Updated**: 2026-02-25

How to install cppcheck, clang-tidy, and configure the environment for running benchmarks alongside sqc.

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

For the latest release, build from source:
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
clang-tidy --version
```

To pin a specific LLVM version (e.g., 18):
```bash
sudo apt install -y clang-18 clang-tidy-18
sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-18 100
sudo update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-18 100
```

---

## 3. Install bear (for compile_commands.json)

Required for clang-tidy on projects with build systems:
```bash
sudo apt install -y bear
```

---

## 4. Juliet Test Suite Setup

Download [NIST Juliet Test Suite v1.3](https://samate.nist.gov/SARD/test-suites/112):
```bash
mkdir -p ~/data/benchmarks
cd ~/data/benchmarks
# Download and extract juliet-test-suite-c
# Expected structure:
#   juliet-test-suite-c/testcases/CWE*/     (118 CWE directories)
#   juliet-test-suite-c/testcasesupport/    (shared helper functions)
```

---

## 5. Real-World Project Setup

```bash
mkdir -p ~/data/comparisons
cd ~/data/comparisons

# libcrc
git clone https://github.com/lammertb/libcrc.git

# sqlite
# Download from https://sqlite.org/src/ or fossil clone

# mosquitto
git clone https://github.com/eclipse/mosquitto.git
sudo apt install -y libcjson-dev  # required dependency

# curl
git clone https://github.com/curl/curl.git
sudo apt install -y libpsl-dev  # required for curl 8.19+

# hostap
git clone git://w1.fi/hostap.git

# Create output directories
mkdir -p ~/data/comparisons/results/{sqc,cppcheck,clang-tidy}/{libcrc,sqlite,mosquitto,curl,hostap}
```

---

## 6. Running Each Tool

### sqc

```bash
# Build
cd ~/data/tools_sqc
cargo build --release

# Basic run
./target/release/sqc /path/to/source/ --export results.json

# With cross-file context
./target/release/sqc /path/to/source/ -d /path/to/source/ --export results.json

# IMPORTANT: When running from outside the sqc repo, pass --manifest explicitly
./target/release/sqc /path/to/source/ \
  --manifest ~/data/tools_sqc/rules_templates/rules-all.toml \
  --export results.json
```

### cppcheck

```bash
# Basic run with XML output
cppcheck --enable=all --std=c11 --xml --xml-version=2 \
  --suppress=missingIncludeSystem \
  -I /path/to/include \
  /path/to/source/ \
  2> results.xml

# IMPORTANT: cppcheck writes XML to stderr, not stdout
# IMPORTANT: Never pass -I /usr/include — causes parse errors in system headers
# IMPORTANT: --addon=cert is not available on Ubuntu 24.04; --enable=all includes CERT checks
```

### clang-tidy

```bash
# Option A: With compile_commands.json (recommended)
cd /path/to/project
make clean && bear -- make -j$(nproc)
run-clang-tidy -checks='-*,cert-*,clang-analyzer-*' -p .

# Option B: Direct file scan (no build required)
find /path/to/source/ -name '*.c' | \
  xargs -P $(nproc) -I{} clang-tidy \
    -checks='-*,cert-*,clang-analyzer-*' \
    {} -- -std=c11 -I /path/to/include

# IMPORTANT: bear requires make clean first — it only captures invocations during an actual build
# IMPORTANT: Verify compile_commands.json is non-empty before running clang-tidy
```

---

## 7. Per-Project Commands

### libcrc

```bash
# sqc
~/data/tools_sqc/target/release/sqc ~/data/comparisons/libcrc \
  -d ~/data/comparisons/libcrc \
  --manifest ~/data/tools_sqc/rules_templates/rules-all.toml \
  --export ~/data/comparisons/results/sqc/libcrc/results.json

# cppcheck
cppcheck --enable=all --std=c11 --xml --xml-version=2 \
  --suppress=missingIncludeSystem \
  -I ~/data/comparisons/libcrc/include \
  ~/data/comparisons/libcrc/src/ \
  2> ~/data/comparisons/results/cppcheck/libcrc/results.xml

# clang-tidy
cd ~/data/comparisons/libcrc && make clean && bear -- make
run-clang-tidy -checks='-*,cert-*,clang-analyzer-*' -p . \
  2>&1 | tee ~/data/comparisons/results/clang-tidy/libcrc/results.txt
```

### sqlite

```bash
# sqc
~/data/tools_sqc/target/release/sqc ~/data/comparisons/sqlite \
  -d ~/data/comparisons/sqlite \
  --manifest ~/data/tools_sqc/rules_templates/rules-all.toml \
  --export ~/data/comparisons/results/sqc/sqlite/results.json

# cppcheck
cppcheck --enable=all --std=c11 --xml --xml-version=2 \
  --suppress=missingIncludeSystem \
  -I ~/data/comparisons/sqlite/src \
  ~/data/comparisons/sqlite/src/ \
  2> ~/data/comparisons/results/cppcheck/sqlite/results.xml

# clang-tidy
cd ~/data/comparisons/sqlite && ./configure && make clean && bear -- make -j$(nproc)
run-clang-tidy -checks='-*,cert-*,clang-analyzer-*' -p . \
  2>&1 | tee ~/data/comparisons/results/clang-tidy/sqlite/results.txt
```

### mosquitto

```bash
# sqc
~/data/tools_sqc/target/release/sqc ~/data/comparisons/mosquitto \
  -d ~/data/comparisons/mosquitto \
  --manifest ~/data/tools_sqc/rules_templates/rules-all.toml \
  --export ~/data/comparisons/results/sqc/mosquitto/results.json

# cppcheck
cppcheck --enable=all --std=c11 --xml --xml-version=2 \
  --suppress=missingIncludeSystem \
  -I ~/data/comparisons/mosquitto/include \
  -I ~/data/comparisons/mosquitto/common \
  -i ~/data/comparisons/mosquitto/deps \
  ~/data/comparisons/mosquitto/lib/ ~/data/comparisons/mosquitto/src/ \
  2> ~/data/comparisons/results/cppcheck/mosquitto/results.xml

# clang-tidy
cmake -S ~/data/comparisons/mosquitto -B ~/data/comparisons/mosquitto/build \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DWITH_TLS=OFF -DWITH_WEBSOCKETS=OFF -DWITH_TESTS=OFF
cmake --build ~/data/comparisons/mosquitto/build --clean-first -j$(nproc)
run-clang-tidy -checks='-*,cert-*,clang-analyzer-*' -p ~/data/comparisons/mosquitto/build \
  2>&1 | tee ~/data/comparisons/results/clang-tidy/mosquitto/results.txt
```

### curl

```bash
# sqc
~/data/tools_sqc/target/release/sqc ~/data/comparisons/curl \
  -d ~/data/comparisons/curl \
  --manifest ~/data/tools_sqc/rules_templates/rules-all.toml \
  --export ~/data/comparisons/results/sqc/curl/results.json

# cppcheck
cppcheck --enable=all --std=c11 --xml --xml-version=2 \
  --suppress=missingIncludeSystem \
  -I ~/data/comparisons/curl/include -I ~/data/comparisons/curl/lib \
  ~/data/comparisons/curl/lib/ ~/data/comparisons/curl/src/ \
  2> ~/data/comparisons/results/cppcheck/curl/results.xml

# clang-tidy
cmake -S ~/data/comparisons/curl -B ~/data/comparisons/curl/build \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DBUILD_SHARED_LIBS=OFF \
  -DCURL_USE_OPENSSL=OFF -DCURL_DISABLE_LDAP=ON \
  -DUSE_LIBIDN2=OFF -DUSE_NGHTTP2=OFF -DCURL_ZSTD=OFF
cmake --build ~/data/comparisons/curl/build --clean-first -j$(nproc)
run-clang-tidy -checks='-*,cert-*,clang-analyzer-*' -p ~/data/comparisons/curl/build \
  2>&1 | tee ~/data/comparisons/results/clang-tidy/curl/results.txt
```

### hostap

```bash
# sqc
~/data/tools_sqc/target/release/sqc ~/data/comparisons/hostap \
  -d ~/data/comparisons/hostap/src \
  -d ~/data/comparisons/hostap/wpa_supplicant \
  --manifest ~/data/tools_sqc/rules_templates/rules-all.toml \
  --export ~/data/comparisons/results/sqc/hostap/results.json

# cppcheck
cppcheck --enable=all --std=c11 --xml --xml-version=2 \
  --suppress=missingIncludeSystem \
  -I ~/data/comparisons/hostap/src \
  -I ~/data/comparisons/hostap/src/utils \
  -I ~/data/comparisons/hostap/src/common \
  ~/data/comparisons/hostap/src/ ~/data/comparisons/hostap/wpa_supplicant/ \
  2> ~/data/comparisons/results/cppcheck/hostap/results.xml

# clang-tidy (requires .config for wpa_supplicant)
cd ~/data/comparisons/hostap/wpa_supplicant
cat > .config <<'EOF'
CONFIG_DRIVER_NL80211=y
CONFIG_LIBNL32=y
CONFIG_IEEE8021X_EAPOL=y
CONFIG_EAP_MD5=y
EOF
make clean 2>/dev/null; bear -- make -j$(nproc) wpa_supplicant 2>/dev/null || true
cd ~/data/comparisons/hostap
python3 gen_compile_commands.py -o compile_commands.json wpa_supplicant/
run-clang-tidy -checks='-*,cert-*,clang-analyzer-*' -p . \
  2>&1 | tee ~/data/comparisons/results/clang-tidy/hostap/results.txt
```

---

## 8. Verifying Results

### Quick Sanity Check

```bash
PROJECT=libcrc
RESULTS=~/data/comparisons/results

echo "=== sqc ==="
python3 -c "
import json, collections
data = json.load(open('$RESULTS/sqc/$PROJECT/results.json'))
rules = collections.Counter(v['rule_id'] for v in data)
print(f'Total: {len(data)} findings, {len(rules)} rules')
for r, c in rules.most_common(10): print(f'  {r}: {c}')
"

echo "=== cppcheck ==="
python3 -c "
import xml.etree.ElementTree as ET, collections
errors = ET.parse('$RESULTS/cppcheck/$PROJECT/results.xml').getroot().findall('.//error')
ids = collections.Counter(e.get('id') for e in errors)
print(f'Total: {len(errors)} findings')
for i, c in ids.most_common(10): print(f'  {i}: {c}')
"

echo "=== clang-tidy ==="
grep -c "warning:\|error:" $RESULTS/clang-tidy/$PROJECT/results.txt
```

### Known Pitfalls

| Pitfall | Symptom | Fix |
|---------|---------|-----|
| sqc manifest not found | `Failed to read manifest file` | Always pass `--manifest` with absolute path |
| cppcheck `--addon=cert` missing | `Did not find addon cert.py` | Remove it; `--enable=all` includes CERT checks |
| cppcheck `-I /usr/include` | `syntaxError: syntax error @ stdlib.h` | Never pass system include paths to cppcheck |
| Empty compile_commands.json | clang-tidy shows no diagnostics | Run `make clean` before `bear -- make` |
| clang-tidy "non-empty but no findings" | File has Enabled checks block only | Grep for `warning:` — must be > 0 |

---

## 9. Output Format Reference

### cppcheck XML (v2)
```xml
<error id="bufferAccessOutOfBounds" severity="error"
       msg="Array 'arr[10]' accessed at index 10..." cwe="788">
  <location file="example.c" line="5" column="5"/>
</error>
```

### clang-tidy Text
```
example.c:5:5: warning: ... [cert-arr30-c]
```

### sqc JSON
```json
[{"rule_id": "ARR30-C", "severity": "High", "file": "example.c", "line": 5, "message": "..."}]
```

### sqc SARIF
SARIF 2.1.0 compatible output with `ruleId` matching CERT C rule IDs.

---

## 10. Distributed Benchmarking with GNU Parallel

For fast re-benchmarking across multiple machines after rule changes.

**Key principle**: cppcheck and clang-tidy results are stable across sqc changes — run them once and cache. Only sqc needs re-running.

### Prerequisites

```bash
sudo apt install -y parallel
parallel --citation <<< "will cite" 2>/dev/null || true
```

SSH key auth from head node to all compute nodes:
```bash
ssh-keygen -t ed25519 -f ~/.ssh/id_benchmark -N ""
for node in node1 node2 node3 node4 node5 node6 node7 node8; do
  ssh-copy-id -i ~/.ssh/id_benchmark.pub $node
done
```

### Node File

```
# ~/.benchmark_nodes
brandon@node1/8
brandon@node2/8
brandon@node3/8
brandon@node4/8
```

Verify connectivity:
```bash
parallel --sshloginfile $NODES_FILE --nonall \
  'echo "$(hostname): $(nproc) cores"'
```

### Fast Re-Benchmark Workflow

```bash
# 1. Rebuild sqc
cd ~/data/tools_sqc && cargo build --release

# 2. If no shared FS, push binary
parallel --sshloginfile $NODES_FILE --nonall \
  "rsync -az $SQC_BIN {}/sqc_bin"

# 3. Generate CWE list and distribute
find $JULIET_DIR -maxdepth 1 -type d -name 'CWE*' | sort > /tmp/cwe_dirs.txt

# 4. Run in parallel across nodes
parallel --sshloginfile $NODES_FILE -a /tmp/cwe_dirs.txt \
  "$SQC_BIN {} -d $JULIET_DIR -d $JULIET_SUPPORT --export $RESULTS_DIR/sqc/juliet/{/.}.csv"
```
