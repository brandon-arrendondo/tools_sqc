# Manifest Module

This module handles the configuration and management of CERT C rules through TOML-based manifest files.

## Overview

The manifest system provides a flexible way to:
- Enable/disable specific CERT rules
- Configure rule severity levels
- Set project-specific compliance requirements
- Document rule exceptions and customizations

## Manifest File Format

### Basic Structure
```toml
[metadata]
name = "Project Security Rules"
version = "1.0.0"
description = "CERT C compliance rules for our project"
cert_version = "2016"
created = "2024-01-15"
author = "Security Team"

[global]
default_severity = "Medium"
fail_on_high = true
max_violations = 100

[rules.ARR30-C]
enabled = true
severity = "High"
description = "Do not form or use out-of-bounds pointers"
category = "Rule"
cert_id = "ARR30-C"
custom_message = "Array bounds violation detected"
```

## Components

### Metadata Section
Project and configuration metadata:
- `name` - Manifest name
- `version` - Manifest version (semver)
- `description` - Purpose and scope
- `cert_version` - CERT standard version
- `created` - Creation date
- `author` - Maintainer information

### Global Settings
Default configurations:
- `default_severity` - Default severity for rules
- `fail_on_high` - Exit with error on high severity
- `max_violations` - Maximum violations before stopping
- `output_format` - Default export format

### Rule Configuration
Per-rule settings:
- `enabled` - Whether to check this rule
- `severity` - Override severity (High/Medium/Low)
- `description` - Rule description
- `category` - Rule category (Rule/Recommendation)
- `cert_id` - Official CERT identifier
- `custom_message` - Custom violation message
- `exceptions` - List of file patterns to exclude

## Core Functions

### `load_manifest()`
Loads and validates manifest:
- Parses TOML file
- Validates schema
- Applies defaults
- Returns RuleManifest struct

### `create_default_manifest()`
Generates template manifest:
- Includes all available rules
- Sets recommended defaults
- Adds documentation comments
- Writes to specified path

### `validate_manifest()`
Ensures manifest integrity:
- Checks required fields
- Validates severity values
- Verifies rule IDs exist
- Reports configuration errors

### `merge_manifests()`
Combines multiple manifests:
- Inheritance support
- Override precedence
- Conflict resolution
- Profile composition

## Manifest Profiles

### Security-Critical Profile
```toml
[profile]
name = "security-critical"
base = "strict"

[rules]
# All rules enabled with high severity
default_enabled = true
default_severity = "High"
```

### Performance Profile
```toml
[profile]
name = "performance"

[rules]
# Only critical security rules
ARR30-C.enabled = true
MEM30-C.enabled = true
# Others disabled for speed
```

### Development Profile
```toml
[profile]
name = "development"

[rules]
# More lenient during development
default_severity = "Low"
fail_on_high = false
```

## Rule Categories

### Severity Levels
- **High** - Critical security vulnerabilities
- **Medium** - Important safety issues
- **Low** - Code quality recommendations

### Rule Types
- **Rule** - Mandatory compliance requirement
- **Recommendation** - Best practice suggestion

## Usage Examples

### Loading Custom Manifest
```rust
let manifest = load_manifest("custom-rules.toml")?;
let rules = manifest.get_enabled_rules();
```

### Creating Project Manifest
```rust
create_default_manifest("project-rules.toml")?;
// Edit the file to customize
```

### Programmatic Configuration
```rust
let mut manifest = RuleManifest::new();
manifest.enable_rule("ARR30-C", Severity::High);
manifest.disable_rule("PRE31-C");
```

## Integration Points

- **Main Module** - CLI argument parsing
- **Analyze Module** - Rule enablement checking
- **Rules Module** - Severity assignment

## Error Handling

Comprehensive error handling for:
- Missing manifest files
- Invalid TOML syntax
- Unknown rule identifiers
- Incompatible configurations
- Version mismatches

## Best Practices

1. Version control manifest files
2. Document rule exceptions
3. Review manifest changes in code review
4. Use profiles for different environments
5. Regularly update to latest CERT standards