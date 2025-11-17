# CERT C Rules Templates

This directory contains individual TOML configuration files for each CERT C rule supported by the SQC tool. Each file contains the configuration for a single rule, making it easy to test individual rules or create custom rule sets.

## Usage

### Testing a Single Rule
To test a specific rule, use the individual TOML file:
```bash
# Test only the MEM33-C rule
sqc --config rules_templates/MEM33-C.toml /path/to/code

# Test only array bounds checking
sqc --config rules_templates/ARR30-C.toml /path/to/code
```

### Creating Custom Rule Sets
You can create custom rule configurations by copying and combining rules:
```bash
# Create a memory-focused rule set
cat rules_templates/MEM*.toml > memory_rules.toml

# Create an array-focused rule set
cat rules_templates/ARR*.toml > array_rules.toml
```

### Rule Categories

**Array Rules (ARR)**
- ARR00-C.toml - Understand how arrays are used (Recommendation)
- ARR30-C.toml - Do not form or use out-of-bounds pointers or array subscripts
- ARR32-C.toml - Ensure size arguments for variable length arrays are in a valid range
- ARR36-C.toml - Do not subtract or compare two pointers that do not refer to the same array
- ARR37-C.toml - Do not add or subtract an integer to a pointer to a non-array object
- ARR38-C.toml - Guarantee that library functions do not form invalid pointers
- ARR39-C.toml - Do not add or subtract a scaled integer to a pointer

**String Rules (STR)**
- STR30-C.toml - Do not attempt to modify string literals
- STR31-C.toml - Guarantee that storage for strings has sufficient space for character data and the null terminator

**Declaration Rules (DCL)**
- DCL00-C.toml - Const-qualify immutable objects (Recommendation)

**Error Handling Rules (ERR)**
- ERR33-C.toml - Detect and handle standard library errors

**Expression Rules (EXP)**
- EXP33-C.toml - Do not read uninitialized memory
- EXP34-C.toml - Do not dereference null pointers

**Preprocessor Rules (PRE)**
- PRE30-C.toml - Do not create a universal character name through concatenation
- PRE31-C.toml - Avoid side effects in arguments to unsafe macros
- PRE32-C.toml - Do not use preprocessor directives in invocations of function-like macros

**Integer Rules (INT)**
- INT30-C.toml - Ensure that unsigned integer operations do not wrap
- INT32-C.toml - Ensure that operations on signed integers do not result in overflow

**Memory Management Rules (MEM)**
- MEM30-C.toml - Do not access freed memory (Critical severity)
- MEM31-C.toml - Free dynamically allocated memory when no longer needed
- MEM33-C.toml - Allocate and copy structures containing a flexible array member dynamically

**File I/O Rules (FIO)**
- FIO30-C.toml - Exclude user input from format strings
- FIO34-C.toml - Distinguish between characters read from a file and EOF or WEOF

**Miscellaneous Rules (MSC)**
- MSC00-C.toml - Compile cleanly at high warning levels (Recommendation, disabled by default)

## Rule Status
- **Enabled by default**: All rules except MSC00-C
- **Severity levels**: Critical, High, Medium, Low
- **Categories**: Rule, Recommendation

## Master Configuration File

The complete rule set is available in `rules-all.toml` which contains all rules in a single file. This is equivalent to combining all individual rule files.

```bash
# Use the complete rule set (all rules enabled/disabled as configured)
sqc --config rules_templates/rules-all.toml /path/to/code

# Compare with testing individual rules
sqc --config rules_templates/MEM33-C.toml /path/to/code  # Single rule only
```

## Notes
- Each file includes complete metadata and rule configuration
- Severity and enabled status can be modified in each file
- The master configuration file `rules-all.toml` contains all rules in a single file
- Based on CERT C Coding Standard 2016 edition