# Export Module

This module handles exporting violation reports to various file formats for documentation, tracking, and integration with other tools.

## Supported Formats

### CSV Export
Generates comma-separated values files with:
- Standard CSV format compatible with Excel, Google Sheets, etc.
- Headers: File, Line, Column, Rule, Severity, Message
- UTF-8 encoding with proper escaping
- Suitable for data analysis and reporting

### Excel Export (XLSX)
Creates formatted Excel workbooks featuring:
- Styled headers with bold formatting
- Color-coded severity levels (red/yellow/blue)
- Auto-sized columns for readability
- Hyperlinked rule IDs to CERT documentation
- Summary statistics sheet (optional)

## Core Functions

### `export_all_violations()`
Main export orchestrator that:
- Accepts violations vector and output path
- Determines format from file extension
- Delegates to appropriate format handler
- Returns success/error status

### `export_to_csv()`
CSV generation that:
- Creates CSV writer with headers
- Iterates through violations
- Properly escapes special characters
- Handles file I/O errors gracefully

### `export_to_excel()`
Excel workbook creation that:
- Initializes workbook and worksheet
- Applies formatting and styles
- Writes violation data with colors
- Adds metadata and timestamps

## Export File Structure

### CSV Format
```csv
File,Line,Column,Rule,Severity,Message
src/main.c,42,15,ARR30-C,High,"Array index out of bounds"
src/utils.c,108,23,STR31-C,Medium,"String buffer too small"
```

### Excel Format
- **Sheet 1: Violations** - Main violation data with formatting
- **Sheet 2: Summary** (optional) - Statistics by rule and severity
- **Sheet 3: Suppressions** (optional) - List of suppressed violations

## Usage Examples

```rust
// Export to CSV
export_all_violations(&violations, "report.csv")?;

// Export to Excel
export_all_violations(&violations, "report.xlsx")?;

// Custom export with filters
let filtered = violations.iter()
    .filter(|v| v.severity == "High")
    .collect();
export_all_violations(&filtered, "critical.csv")?;
```

## Integration Points

- **UI Module** - Triggered by export commands (e/E keys)
- **Analyze Module** - Receives violation data
- **Main** - Command-line export flag processing

## Error Handling

Robust error handling for:
- File permission issues
- Disk space constraints
- Invalid file paths
- Format-specific errors (Excel limits, etc.)

## Performance

- Streaming writes for large datasets
- Buffered I/O for efficiency
- Memory-conscious for Excel generation
- Progress indication for long exports

## Future Enhancements

Potential additions:
- JSON export for tool integration
- HTML reports with interactive features
- Markdown reports for documentation
- SARIF format for IDE integration
- Custom templates for reports