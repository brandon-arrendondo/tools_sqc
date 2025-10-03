# Analyze Module

This module contains the core analysis engine that orchestrates the security scanning process and manages violation suppression.

## Components

### `mod.rs` - Analysis Orchestration
The main analysis module that coordinates:
- File discovery and filtering
- C code parsing with tree-sitter
- Rule execution across the AST
- Violation aggregation and deduplication
- Integration with suppression system

### `suppression.rs` - Violation Suppression System
Implements SHA-256 based suppression management:
- Generation of unique hashes for violations
- Loading and saving suppression files (`.sqc-suppress.toml`)
- Filtering of suppressed violations from results
- Audit trail for suppression reasons

## Analysis Pipeline

1. **Project Initialization**
   - Load manifest configuration
   - Discover C source files (.c, .h)
   - Filter based on gitignore rules

2. **File Processing**
   - Parse each C file into an AST using tree-sitter
   - Walk the AST nodes recursively

3. **Rule Application**
   - Apply each enabled CERT rule to AST nodes
   - Collect violations with context information
   - Calculate severity based on rule configuration

4. **Suppression Filtering**
   - Load suppression file if exists
   - Generate SHA-256 hash for each violation
   - Filter out matching suppressed violations

5. **Result Aggregation**
   - Deduplicate violations
   - Sort by severity and location
   - Prepare for UI display or export

## Key Functions

### `analyze_project()`
Main entry point that:
- Accepts project path and manifest
- Returns vector of violations
- Handles errors gracefully with context

### `handle_generate_suppression()`
Creates suppression file by:
- Running full analysis
- Generating hashes for all violations
- Writing `.sqc-suppress.toml` file

### `apply_suppressions()`
Filters violations by:
- Computing SHA-256 hash from violation data
- Matching against suppression database
- Preserving non-suppressed violations

## Suppression File Format

```toml
[[suppressions]]
file = "src/example.c"
rule = "ARR30-C"
line = 42
column = 15
hash = "a1b2c3d4e5f6..."
reason = "Bounds check performed in calling function"
timestamp = "2024-01-15T10:30:00Z"
```

## Performance Considerations

- Tree-sitter provides incremental parsing for efficiency
- Rules are applied in a single AST traversal
- Suppression lookup uses HashMap for O(1) access
- File I/O is minimized through caching

## Error Handling

The module uses `anyhow::Result` for error propagation with contextual information:
- File access errors
- Parse failures
- Rule execution errors
- Suppression file corruption

## Integration Points

- **Parser Module** - For C code AST generation
- **Rules Module** - For CERT rule execution
- **Manifest Module** - For configuration loading
- **Files Module** - For source file discovery