# SqC - Software Code Quality

A comprehensive terminal-based static analysis tool that validates C code compliance with SEI CERT Coding Standards. SqC provides both interactive and command-line interfaces for analyzing C codebases, identifying security vulnerabilities, and ensuring adherence to industry-standard secure coding practices.

## Features

- **CERT C Compliance**: Validates code against 15 implemented SEI CERT C coding standards
- **Interactive Terminal UI**: Navigate and review violations with a modern TUI built with ratatui
- **Multiple Export Formats**: Export violations to CSV or XLSX for reporting and tracking
- **Violation Suppression**: Suppress false positives with SHA-256 based suppression system
- **Git Integration**: Seamlessly analyze C files in git repositories
- **Configurable Rules**: Enable/disable rules via TOML manifest with per-rule severity settings
- **Fast Analysis**: Tree-sitter based parsing for efficient code analysis
- **Extensible Architecture**: Plugin-style rule system for easy addition of new CERT C rules

## NIST Juliet Benchmark Results

SqC has been benchmarked against the [NIST Juliet Test Suite v1.3](https://samate.nist.gov/SARD/test-suites/112) for C/C++, covering all 118 CWE categories (54,484 test files). Juliet provides ground truth via preprocessor-guarded sections: violations in `OMITBAD` sections are true positives, violations in `OMITGOOD` sections are false positives.

### Aggregate Metrics

| Metric | Value |
|--------|-------|
| **Files Analyzed** | 54,484 |
| **True Positives** | 230,992 |
| **False Positives** | 296,415 |
| **TP Rate** | **43.8%** |
| **CWE Categories** | 106 / 118 with data |

### FP Reduction Progress

Eight rounds of targeted rule improvements plus cross-file analysis reduced false positives by 65% from baseline while improving the true positive rate:

| Round | Fixes | TP | FP | TP Rate | FP Delta |
|-------|-------|---:|---:|--------:|---------:|
| Baseline | -- | 586,539 | 839,341 | 41.1% | -- |
| Round 1 | INT08-C, CON08-C, DCL20-C, ARR38-C | 552,645 | 752,422 | 42.3% | -86,919 |
| Round 2 | EXP33-C, SIG31-C, ARR01-C, DCL30-C, DCL02-C | 555,700 | 736,563 | 43.0% | -15,859 |
| Round 3 | DCL31-C, DCL07-C, FLP34-C | 402,013 | 537,589 | 42.8% | -198,974 |
| Round 4 | EXP12-C, FLP03-C, INT32-C | 363,914 | 492,648 | 42.5% | -44,941 |
| Round 5 | FLP02-C, DCL06-C, INT30-C | 340,894 | 475,813 | 41.7% | -16,835 |
| Round 6 | Cross-file analysis (`-d`) | 247,757 | 327,191 | 43.1% | -148,622 |
| Round 7 | EXP36-C, EXP34-C, ARR37-C | 231,053 | 301,475 | 43.4% | -25,716 |
| **Round 8** | **DCL40-C, FLP32-C, ERR33-C** | **230,992** | **296,415** | **43.8%** | **-5,060** |

**Cumulative**: TP rate 41.1% → 43.8% (+2.7pp), FP reduced by 542,926 (-64.7%).

### Top CWE Detection Rates

| CWE | Category | TP Rate |
|-----|----------|--------:|
| 480 | Use of Incorrect Operator | 91.7% |
| 506 | Embedded Malicious Code | 85.9% |
| 587 | Assignment of Fixed Address to Pointer | 83.3% |
| 617 | Reachable Assertion | 79.2% |
| 197 | Numeric Truncation Error | 78.3% |
| 464 | Data Structure Sentinel Addition | 77.6% |
| 427 | Uncontrolled Search Path Element | 72.8% |
| 78 | OS Command Injection | 71.4% |
| 123 | Write-What-Where Condition | 68.2% |
| 15 | External Control of System/Config | 67.0% |

18 categories achieve >50% TP rate. See [JULIET_BENCHMARK_SUMMARY.md](JULIET_BENCHMARK_SUMMARY.md) for full details, methodology, and per-round fix descriptions.

## Installation

```bash
git clone <repository-url>
cd sqc
cargo build --release
```

## Usage

### Interactive Mode

```bash
# Analyze current directory with interactive UI
./target/release/sqc --interactive

# Analyze specific repository
./target/release/sqc /path/to/repo --interactive

# Use custom rules manifest
./target/release/sqc --manifest custom-rules.toml --interactive
```

### Command Line Mode

```bash
# Basic analysis (non-interactive)
./target/release/sqc /path/to/repo

# With custom manifest
./target/release/sqc /path/to/repo --manifest custom-rules.toml

# Export violations to CSV
./target/release/sqc /path/to/repo --export violations.csv

# Export violations to Excel
./target/release/sqc /path/to/repo --export-xlsx violations.xlsx

# Generate suppression file for current violations
./target/release/sqc /path/to/repo --generate-suppression

# Use suppression file to filter false positives
./target/release/sqc /path/to/repo --suppression .sqc-suppress.toml
```

## Configuration

### Quick Start with Rule Templates

Use the provided rule templates for easy configuration:

```bash
# Use all rules (complete rule set)
sqc --config rules_templates/rules-all.toml /path/to/code

# Test individual rules
sqc --config rules_templates/MEM33-C.toml /path/to/code
sqc --config rules_templates/ARR30-C.toml /path/to/code

# Create custom rule sets
cat rules_templates/MEM*.toml > memory_rules.toml
sqc --config memory_rules.toml /path/to/code
```

### Custom Configuration

Create a custom `sqc-rules.toml` file to configure which CERT C rules to apply:

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

Currently implemented 15 CERT C rules:

### Array Rules (ARR)
- **ARR30-C**: Do not form or use out-of-bounds pointers or array subscripts
- **ARR32-C**: Ensure size arguments for variable-length arrays are in a valid range
- **ARR36-C**: Do not subtract or compare two pointers that do not refer to the same array
- **ARR37-C**: Do not add or subtract an integer to a pointer to a non-array object
- **ARR38-C**: Guarantee that library functions do not form invalid pointers
- **ARR39-C**: Do not add or subtract a scaled integer to a pointer

### Declaration Rules (DCL)
- **DCL00-C**: Const-qualify immutable objects

### Expression Rules (EXP)
- **EXP33-C**: Do not read uninitialized memory

### Integer Rules (INT)
- **INT30-C**: Ensure that unsigned integer operations do not wrap

### Memory Rules (MEM)
- **MEM30-C**: Do not access freed memory

### Preprocessor Rules (PRE)
- **PRE30-C**: Do not create a universal character name through concatenation
- **PRE31-C**: Avoid side effects in arguments to unsafe macros
- **PRE32-C**: Do not use preprocessor directives in invocations of function-like macros

### String Rules (STR)
- **STR31-C**: Guarantee that storage for strings has sufficient space for character data and the null terminator

### Rule Templates

All supported rules are available as individual template files in the `rules_templates/` directory:
- **Individual rule files**: Each rule has its own `.toml` file (e.g., `MEM33-C.toml`, `ARR30-C.toml`)
- **Complete rule set**: `rules-all.toml` contains all rules in a single file
- **Easy testing**: Test individual rules or create custom rule combinations
- **Documentation**: See `rules_templates/README.md` for detailed usage instructions

Additional rules can be easily added by implementing the `CertRule` trait in the `src/rules/cert_c/` directory.

## Interactive UI Controls

### Navigation
- `↑/↓` or `k/j` - Navigate violation list
- `Page Up/Page Down` - Navigate by page
- `Home/End` - Jump to first/last violation
- `Enter` - View detailed violation information

### Actions
- `s` - Scan repository for violations
- `e` - Export violations to CSV
- `E` - Export violations to Excel (XLSX)
- `x` - Suppress selected violation
- `X` - Generate suppression file for all violations
- `h` - Toggle hidden items (show/hide suppressed violations and clean files)
- `f` - Toggle file filter
- `r` - Toggle rule filter
- `q` - Quit application

### Views
- `Tab` - Switch between violations list and details view
- `1-3` - Switch between different panel views

## Project Structure

```
src/
├── main.rs          # CLI entry point and argument parsing
├── prelude.rs       # Common imports and type definitions
├── analyze/         # Core analysis engine
│   ├── mod.rs       # Project analysis orchestration
│   └── suppression.rs # Violation suppression system
├── export/          # Export functionality
│   └── mod.rs       # CSV and XLSX export implementations
├── files/           # File and repository handling
│   └── mod.rs       # Git integration and file discovery
├── manifest/        # Rule configuration system
│   └── mod.rs       # TOML manifest parsing and validation
├── parser/          # C code parsing
│   └── mod.rs       # Tree-sitter C parser integration
├── rules/           # CERT C rule implementations
│   ├── mod.rs       # Rule trait and registry
│   └── cert_c/      # Individual CERT C rule modules
│       ├── arr30_c.rs, arr32_c.rs, ... # Array rules
│       ├── dcl00_c.rs                  # Declaration rules
│       ├── exp33_c.rs                  # Expression rules
│       ├── int30_c.rs                  # Integer rules
│       ├── mem30_c.rs                  # Memory rules
│       ├── pre30_c.rs, pre31_c.rs, ... # Preprocessor rules
│       └── str31_c.rs                  # String rules
├── ui/              # Terminal user interface
│   └── mod.rs       # Ratatui-based interactive UI
└── utility/         # Helper functions
    └── mod.rs       # Common utilities and helpers
```

## Adding New Rules

1. Create a new file in `src/rules/` (e.g., `mem30_c.rs`)
2. Implement the `CertRule` trait
3. Register the rule in `src/rules/mod.rs`
4. Add configuration to your manifest file

Example rule implementation:

```rust
use super::{CertRule, RuleViolation};
use tree_sitter::Node;

pub struct Mem30C;

impl CertRule for Mem30C {
    fn rule_id(&self) -> &'static str {
        "MEM30-C"
    }

    fn description(&self) -> &'static str {
        "Do not access freed memory"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
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

SqC includes a sophisticated suppression system to handle false positives:

### Creating Suppressions
```bash
# Generate suppression file for all current violations
./target/release/sqc --generate-suppression

# Or suppress individual violations in the interactive UI with 'x'
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

1. Create a new file in `src/rules/cert_c/` (e.g., `new_rule.rs`)
2. Implement the `CertRule` trait
3. Register the rule in `src/rules/cert_c/mod.rs`
4. Add tests for your rule
5. Update the manifest template with the new rule

## License

MIT OR Apache-2.0