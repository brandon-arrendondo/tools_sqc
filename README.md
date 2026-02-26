# SqC - Software Code Quality

A comprehensive terminal-based static analysis tool that validates C code compliance with SEI CERT Coding Standards. SqC provides both interactive and command-line interfaces for analyzing C codebases, identifying security vulnerabilities, and ensuring adherence to industry-standard secure coding practices.

## Features

- **CERT C Compliance**: 283 SEI CERT C rules implemented
- **Interactive Terminal UI**: Navigate and review violations with a modern TUI built with ratatui
- **Multiple Export Formats**: CSV, XLSX, JSON, and SARIF 2.1.0 output
- **CI/CD Ready**: Exit codes, severity thresholds, rule filtering, and diff-only mode
- **Violation Suppression**: Suppress false positives with SHA-256 based suppression system
- **Git Integration**: Seamlessly analyze C files in git repositories, with diff-only mode for incremental analysis
- **Cross-File Analysis**: Pre-scan directories for function definitions to reduce false positives
- **Inter-Procedural Analysis**: Function summaries computed during prescan enable cross-function reasoning (null returns, freed parameters, no-return functions)
- **Control-Flow Graphs**: Per-function CFG construction with reaching definitions for path-sensitive analysis
- **Configurable Rules**: Enable/disable rules via TOML manifest with per-rule severity settings
- **Fast Analysis**: Tree-sitter based parsing for efficient code analysis
- **Extensible Architecture**: Plugin-style rule system for easy addition of new CERT C rules

## Benchmark Results

SqC has been benchmarked against the [NIST Juliet Test Suite v1.3](https://samate.nist.gov/SARD/test-suites/112) (54,484 files, 118 CWE categories) and 5 real-world open-source C codebases.

| Metric | Value |
|--------|-------|
| **Juliet TP Rate** | **44.5%** (v0.2.7) |
| **FP Reduction from Baseline** | -74.3% (839K → 216K) |
| **Real-World Projects** | libcrc, sqlite, mosquitto, curl, hostap |

See [JULIET_RESULTS.md](JULIET_RESULTS.md) and [REALWORLD_RESULTS.md](REALWORLD_RESULTS.md) for full details.

## Documentation

| File | Contents |
|------|----------|
| [README.md](README.md) | Tool overview, usage, configuration (this file) |
| [JULIET_RESULTS.md](JULIET_RESULTS.md) | Juliet benchmark data: TP/FP history, per-round fixes, per-CWE results, methodology, competitor comparison |
| [REALWORLD_RESULTS.md](REALWORLD_RESULTS.md) | Real-world codebase results: sqc vs cppcheck vs clang-tidy on 5 OSS projects, d_lib_common case study |
| [BENCHMARK_SUMMARY.md](BENCHMARK_SUMMARY.md) | Overall benchmark strategy, architecture assessment, CI/CD readiness, methodology notes |
| [BENCHMARK_INSTALL.md](BENCHMARK_INSTALL.md) | Installing cppcheck, clang-tidy, setting up Juliet and real-world codebases, per-project commands |
| [BENCHMARK_RUNNING.md](BENCHMARK_RUNNING.md) | Running benchmarks via MCP servers (Juliet and real-world), workflow protocol |
| [PLAN.md](PLAN.md) | Future plans: EXP34-C roadmap, pending FP improvements, architecture evolution, competitor research |
| [CLAUDE.md](CLAUDE.md) | Claude Code project instructions (agent workflows, git rules, benchmark protocol) |

## Installation

```bash
git clone <repository-url>
cd sqc
cargo build --release
```

## Usage

### Command Reference

```
sqc [OPTIONS] [PATH]

Arguments:
  [PATH]  Path to the file, directory, or git repository to analyze [default: .]

Options:
  -m, --manifest <FILE>            Path to the rules manifest file
                                   [default: rules_templates/rules-all.toml]
  -i, --interactive                Run in interactive terminal UI mode
  -e, --export <FILE>              Export violations to file (format by extension:
                                   .csv, .xlsx, .json, .sarif, .sarif.json)
      --generate-suppression <FILE:LINE:RULE>
                                   Generate suppression entry for a specific violation
  -d, --directories <DIR>          Additional directories to pre-scan for function
                                   definitions (repeatable; enables cross-file context)
      --fail-on-violation          Exit with code 1 if any violations are found
      --fail-on-severity <LEVEL>   Exit with code 1 if any violation meets or exceeds
                                   this severity [Low, Medium, High, Critical]
      --min-severity <LEVEL>       Only report violations at or above this severity
                                   [Low, Medium, High, Critical]
      --rules <RULE1,RULE2,...>    Only report violations from these rules (comma-separated)
      --diff                       Only analyze modified/new C files (requires git repo)
  -h, --help                       Print help
  -V, --version                    Print version
```

### Interactive Mode

```bash
# Analyze current directory with interactive UI
./target/release/sqc --interactive

# Analyze specific repository
./target/release/sqc /path/to/repo --interactive

# Use custom rules manifest
./target/release/sqc --manifest custom-rules.toml --interactive

# Interactive with cross-file context
./target/release/sqc /path/to/repo -d /path/to/repo --interactive
```

### Command Line Mode

```bash
# Basic analysis — prints violations to stdout
sqc /path/to/repo

# With custom manifest
sqc /path/to/repo --manifest custom-rules.toml

# Export violations to various formats
sqc /path/to/repo --export violations.csv
sqc /path/to/repo --export violations.xlsx
sqc /path/to/repo --export violations.json
sqc /path/to/repo --export violations.sarif       # SARIF 2.1.0
sqc /path/to/repo --export violations.sarif.json   # SARIF (alternate ext)

# Cross-file analysis (pre-scans directories for function definitions)
sqc /path/to/repo -d /path/to/repo -d /path/to/shared/headers

# Generate suppression entry for a specific violation
sqc --generate-suppression src/main.c:42:ARR30-C
```

### CI/CD Integration

```bash
# Fail the build if any violations are found
sqc /path/to/repo --fail-on-violation

# Fail only on High or Critical severity violations
sqc /path/to/repo --fail-on-severity High

# Filter by severity (only report Medium and above)
sqc /path/to/repo --min-severity Medium

# Only check specific rules
sqc /path/to/repo --rules ARR30-C,MEM30-C,STR31-C

# Only analyze files changed in git (diff mode)
sqc /path/to/repo --diff

# Combine for CI pipeline: diff-only, High+ severity, SARIF output
sqc /path/to/repo --diff --min-severity High --fail-on-severity High --export results.sarif
```

**Exit codes:**
- `0` — Success (no violations, or no flags requiring failure)
- `1` — Violations found (when `--fail-on-violation` or `--fail-on-severity` is set)
- `2` — Analysis error (invalid path, bad manifest, etc.)

#### Example CI Workflows

Ready-to-use CI pipeline configurations are included in the repository:

- **GitHub Actions:** [`.github/workflows/sqc-analysis.yml`](.github/workflows/sqc-analysis.yml) — Runs diff-only analysis on PRs and full scans on push to `main`. Uploads SARIF results to [GitHub Code Scanning](https://docs.github.com/en/code-security/code-scanning) via `github/codeql-action/upload-sarif`.
- **Azure DevOps:** [`ci/azure-pipelines.yml`](ci/azure-pipelines.yml) — Same two-mode pattern (PR = diff-only, push = full scan). Publishes SARIF as a build artifact.

Both workflows use `--fail-on-severity High` to gate the pipeline on high-severity findings, and `--min-severity Medium` to filter out low-severity noise.

## Configuration

### Manifest File

The manifest TOML file controls which rules are active and their severity. The default manifest (`rules_templates/rules-all.toml`) enables all 283 rules.

```bash
# Use default (all rules enabled)
sqc /path/to/code

# Use a custom manifest
sqc --manifest my-rules.toml /path/to/code
```

### Custom Manifest Format

```toml
[metadata]
name = "My Project Rules"
version = "1.0.0"
description = "Custom CERT C rules for my project"
cert_version = "2016"

[rules.ARR30-C]
enabled = true
severity = "High"
description = "Do not form or use out-of-bounds pointers or array subscripts"
category = "Rule"
cert_id = "ARR30-C"

[rules.STR31-C]
enabled = false  # Disable this rule
severity = "Medium"
description = "Guarantee that storage for strings has sufficient space"
category = "Rule"
cert_id = "STR31-C"
```

## Supported CERT C Rules

283 rules implemented across 17 CERT C categories:

| Category | Count | Rules |
|----------|------:|-------|
| **API** | 9 | API00-C, API01-C, API02-C, API03-C, API04-C, API05-C, API07-C, API09-C, API10-C |
| **ARR** | 9 | ARR00-C, ARR01-C, ARR02-C, ARR30-C, ARR32-C, ARR36-C, ARR37-C, ARR38-C, ARR39-C |
| **CON** | 23 | CON01–CON09-C, CON30–CON41-C, CON43-C, CON50-C |
| **DCL** | 31 | DCL00–DCL23-C, DCL30-C, DCL31-C, DCL36–DCL41-C |
| **ENV** | 8 | ENV01-C, ENV02-C, ENV03-C, ENV30–ENV34-C |
| **ERR** | 11 | ERR00-C, ERR01-C, ERR02-C, ERR04-C, ERR05-C, ERR06-C, ERR07-C, ERR30-C, ERR32-C, ERR33-C, ERR34-C |
| **EXP** | 31 | EXP00–EXP47-C (selected) |
| **FIO** | 35 | FIO01–FIO51-C (selected) |
| **FLP** | 13 | FLP00–FLP37-C (selected) |
| **INT** | 23 | INT00–INT36-C (selected) |
| **MEM** | 17 | MEM00–MEM36-C (selected) |
| **MSC** | 8 | MSC30-C, MSC32-C, MSC33-C, MSC37–MSC41-C |
| **POS** | 20 | POS01–POS54-C (selected) |
| **PRE** | 16 | PRE00–PRE13-C, PRE30-C, PRE31-C, PRE32-C |
| **SIG** | 7 | SIG00-C, SIG01-C, SIG02-C, SIG30-C, SIG31-C, SIG34-C, SIG35-C |
| **STR** | 16 | STR00–STR38-C (selected) |
| **WIN** | 6 | WIN00–WIN04-C, WIN30-C |

For the full list, see the rule source files in `src/rules/cert_c/` or the complete manifest at `rules_templates/rules-all.toml`.

## Interactive UI Controls

The interactive TUI (`--interactive`) has two tabs: **Violations** and **Configuration**.

### Navigation (both tabs)
| Key | Action |
|-----|--------|
| `↑` / `↓` | Move up/down one item |
| `Page Up` / `Page Down` | Move up/down by 10 items |
| `←` / `→` | Collapse / expand group |
| `q` / `Esc` | Quit |

### Tab Switching
| Key | Action |
|-----|--------|
| `v` | Switch to Violations tab |
| `c` | Switch to Configuration tab |
| `Tab` | Toggle focus between violations list and file preview panel |

### Violations Tab — Selection
| Key | Action |
|-----|--------|
| `Space` | Toggle checkbox on violation; expand/collapse group header |
| `a` | Select all violations |
| `Shift+A` | Select all violations in current group (grouped modes only) |
| `n` | Deselect all violations |
| `Shift+N` | Deselect all violations in current group (grouped modes only) |

### Violations Tab — Actions
| Key | Action |
|-----|--------|
| `s` | Scan repository for violations |
| `i` | Suppress checked violations (writes to `.sqc-suppress.toml`) |
| `e` | Export checked violations to file (prompts for path; CSV format) |
| `h` | Toggle visibility of suppressed violations and clean files |
| `p` | Toggle file preview panel |

### Violations Tab — Sorting / Grouping
| Key | Action |
|-----|--------|
| `1` | Default sort order |
| `2` | Group / sort by violation ID (CERT rule) |
| `3` | Group / sort by file path |
| `4` | Group / sort by filename |
| `r` | Reverse sort direction |

### Configuration Tab
| Key | Action |
|-----|--------|
| `Space` | Toggle rule enabled/disabled |
| `e` | Save configuration to file (prompts for path) |
| `←` / `→` | Collapse / expand rule category group |

## Project Structure

```
src/
├── main.rs          # CLI entry point and argument parsing
├── prelude.rs       # Common imports and type definitions
├── analyze/         # Core analysis engine
│   ├── mod.rs       # Project analysis orchestration
│   ├── cfg.rs       # Control-flow graph construction
│   ├── context.rs   # Cross-file project context
│   ├── dataflow.rs  # Reaching definitions analysis
│   ├── function_summary.rs # Inter-procedural function summaries
│   ├── prescan.rs   # Directory pre-scanning for cross-file context
│   └── suppression.rs # Violation suppression system
├── export/          # Export functionality
│   └── mod.rs       # CSV, XLSX, JSON, and SARIF export
├── files/           # File and repository handling
│   └── mod.rs       # Git integration and file discovery
├── manifest/        # Rule configuration system
│   └── mod.rs       # TOML manifest parsing and validation
├── parser/          # C code parsing
│   └── mod.rs       # Tree-sitter C parser integration
├── progress.rs      # CLI progress reporting
├── rules/           # CERT C rule implementations
│   ├── mod.rs       # Rule trait and registry
│   └── cert_c/      # Individual CERT C rule modules (17 categories)
│       ├── API/     # API rules (9 rules)
│       ├── ARR/     # Array rules (9 rules)
│       ├── CON/     # Concurrency rules (23 rules)
│       ├── DCL/     # Declaration rules (31 rules)
│       ├── ENV/     # Environment rules (8 rules)
│       ├── ERR/     # Error handling rules (11 rules)
│       ├── EXP/     # Expression rules (31 rules)
│       ├── FIO/     # I/O rules (35 rules)
│       ├── FLP/     # Floating point rules (13 rules)
│       ├── INT/     # Integer rules (23 rules)
│       ├── MEM/     # Memory rules (17 rules)
│       ├── MSC/     # Miscellaneous rules (8 rules)
│       ├── POS/     # POSIX rules (20 rules)
│       ├── PRE/     # Preprocessor rules (16 rules)
│       ├── SIG/     # Signal rules (7 rules)
│       ├── STR/     # String rules (16 rules)
│       └── WIN/     # Windows rules (6 rules)
├── ui/              # Terminal user interface
│   └── mod.rs       # Ratatui-based interactive UI
└── utility/         # Helper functions
    └── mod.rs       # Common utilities and helpers
```

## Adding New Rules

1. Create a directory and implementation file in `src/rules/cert_c/CATEGORY/RULE-ID/` (e.g., `src/rules/cert_c/MEM/MEM30-C/mem30_c.rs`)
2. Implement the `CertRule` trait
3. Register the rule in `src/rules/cert_c/mod.rs`
4. Add the rule entry to `rules_templates/rules-all.toml`

Example rule implementation:

```rust
use crate::prelude::*;

pub struct Mem30C;

impl CertRule for Mem30C {
    fn rule_id(&self) -> &'static str {
        "MEM30-C"
    }

    fn description(&self) -> &'static str {
        "Do not access freed memory"
    }

    fn check(&self, node: &Node, source: &str, _context: &ProjectContext) -> Vec<RuleViolation> {
        // Implementation here
        Vec::new()
    }
}
```

## Dependencies

### Core Dependencies
- `clap` (4.5) - Command line argument parsing
- `ratatui` (0.28) - Modern terminal user interface
- `crossterm` (0.28) - Cross-platform terminal manipulation

### Analysis & Parsing
- `tree-sitter` (0.22) - Fast incremental parsing
- `tree-sitter-c` (0.21) - C language grammar
- `regex` (1.10) - Pattern matching for rule implementations

### Data Management
- `git2` (0.19) - Git repository integration
- `serde` (1.0) - Serialization framework
- `serde_json` (1.0) - JSON and SARIF export
- `toml` (0.8) - Configuration file parsing
- `csv` (1.3) - CSV export functionality
- `rust_xlsxwriter` (0.79) - Excel file generation

### Utility
- `anyhow` (1.0) - Error handling
- `thiserror` (1.0) - Custom error types
- `walkdir` (2.5) - Recursive directory traversal
- `sha2` (0.10) - SHA-256 hashing for suppressions
- `chrono` (0.4) - Date and time handling
- `rfd` (0.14) - Native file dialogs

## Suppression System

SqC includes a suppression system to handle false positives:

### Creating Suppressions
```bash
# Generate suppression entry for a specific violation (file:line:rule)
./target/release/sqc --generate-suppression src/main.c:42:ARR30-C

# Or in interactive mode: check violations with Space, then press 'i' to suppress
```

### Suppression File Format
Suppressions are stored in `.sqc-suppress.toml` with SHA-256 hashes:
```toml
[[suppressions]]
file = "src/example.c"
rule = "ARR30-C"
line = 42
hash = "a1b2c3d4..."
reason = "False positive: bounds check performed earlier"
```

## Contributing

Contributions are welcome! To add a new CERT C rule:

1. Create `src/rules/cert_c/CATEGORY/RULE-ID/rule_id_c.rs` with the rule implementation
2. Implement the `CertRule` trait
3. Register the rule in `src/rules/cert_c/mod.rs`
4. Add test cases as `.c` files in `tests/`
5. Add the rule entry to `rules_templates/rules-all.toml`

## License

MIT OR Apache-2.0