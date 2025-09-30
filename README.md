# SqC - Software Code Quality

A terminal-based C code quality checker that validates compliance with SEI CERT Coding Standards for C.

## Features

- **CERT C Compliance**: Checks C code against SEI CERT coding standards
- **Interactive Terminal UI**: Navigate violations with an ncurses-style interface
- **Git Integration**: Analyzes C files in git repositories
- **Configurable Rules**: Enable/disable rules via TOML manifest
- **Extensible Architecture**: Easy to add new CERT C rules

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
```

## Configuration

Create a `sqc-rules.toml` file to configure which CERT C rules to apply:

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

Currently implemented rules:

- **ARR30-C**: Do not form or use out-of-bounds pointers or array subscripts
- **STR31-C**: Guarantee that storage for strings has sufficient space for character data and the null terminator

Additional rules can be easily added by implementing the `CertRule` trait.

## Interactive Controls

- `s` - Scan repository for violations
- `↑/↓` - Navigate violation list
- `q` - Quit

## Project Structure

```
src/
├── main.rs          # CLI entry point
├── rules/           # CERT C rule implementations
│   ├── mod.rs       # Rule registry and trait
│   ├── arr30_c.rs   # ARR30-C implementation
│   └── str31_c.rs   # STR31-C implementation
├── manifest/        # Rule configuration system
├── ui/              # Terminal interface
├── git/             # Git repository integration
└── parser/          # C code parsing with tree-sitter
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

- `clap` - Command line argument parsing
- `ratatui` - Terminal user interface
- `tree-sitter` - C code parsing
- `git2` - Git repository integration
- `serde` & `toml` - Configuration management

## License

MIT OR Apache-2.0