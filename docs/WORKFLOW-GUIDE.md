# Claude Code Workflow Guide

This guide explains when to use each Claude Code mode for maximum safety and productivity.

## Overview

The project has **5 permission modes** to prevent accidental cross-rule contamination:

| Mode | Scope | Use When | Command |
|------|-------|----------|---------|
| **Rule-Scoped Impl** | 1 rule impl | Implementing a single rule | `/mode-impl-rule <RULE-ID>` |
| **Rule-Scoped Impl + Utils** | 1 rule impl + utilities | Implementing rule + shared code | `/mode-impl-rule-utils <RULE-ID>` |
| **Rule-Scoped Test** | 1 rule's tests | Writing tests for one rule | `/mode-test-rule <RULE-ID>` |
| **Broad Implementation** | All 283 rule impls | Refactoring across many rules | `/mode-impl` |
| **Broad Test** | All 2,710 tests | Mass test updates | `/mode-test` |

## Decision Tree

```
Need to work on code?
│
├─ Working on a SINGLE RULE?
│  │
│  ├─ Just the rule's code? ──────────> /mode-impl-rule <RULE-ID>
│  │
│  ├─ Rule + utilities? ──────────────> /mode-impl-rule-utils <RULE-ID>
│  │
│  └─ Just the rule's tests? ─────────> /mode-test-rule <RULE-ID>
│
├─ Working on MULTIPLE RULES?
│  │
│  ├─ Implementations? ───────────────> /mode-impl
│  │
│  └─ Tests? ─────────────────────────> /mode-test
│
└─ Need EVERYTHING unlocked? ─────────> ./scripts/claude_mode_reset.sh
```

## Mode Details

### 1. Rule-Scoped Implementation (`/mode-impl-rule`)

**When to use:**
- Implementing a single CERT C rule
- Fixing bugs in one rule
- Refactoring one rule's implementation

**What's unlocked:**
- ✅ `src/rules/cert_c/<CAT>/<RULE-ID>/*_c.rs` (the rule's implementation)
- ✅ `src/rules/cert_c/<CAT>/<RULE-ID>/*.toml` (the rule's metadata)

**What's locked:**
- ❌ All other rule implementations (282 rules)
- ❌ All test files (2,710 files)
- ❌ Utility files

**Example:**
```bash
# Architect
./scripts/claude_mode_impl_rule.sh ARR38-C

# Claude
/mode-impl-rule ARR38-C
# Now implement ARR38-C, everything else is locked
```

**Benefits:**
- Cannot accidentally edit wrong rule
- Focus on single rule
- Safe experimentation

---

### 2. Rule-Scoped Implementation + Utilities (`/mode-impl-rule-utils`)

**When to use:**
- Implementing a rule that needs new utility functions
- Modifying shared code while working on a rule
- Extracting common logic to utilities

**What's unlocked:**
- ✅ `src/rules/cert_c/<CAT>/<RULE-ID>/*_c.rs` (the rule's implementation)
- ✅ `src/rules/cert_c/<CAT>/<RULE-ID>/*.toml` (the rule's metadata)
- ✅ `src/utility/cert_c/*.rs` (utility files)
- ✅ `src/utility/cert_c/mod.rs`, `src/utility/mod.rs`

**What's locked:**
- ❌ All other rule implementations (282 rules)
- ❌ All test files (2,710 files)

**Example:**
```bash
# Architect
./scripts/claude_mode_impl_rule_utils.sh ARR38-C

# Claude
/mode-impl-rule-utils ARR38-C
# Can edit ARR38-C and utilities, other rules locked
```

**Benefits:**
- Can create/modify shared utilities
- Still protects other rules
- Documents intent (rule + infrastructure change)

**Caution:**
- Utility changes affect ALL rules
- Test thoroughly
- Consider impact on other rules

---

### 3. Rule-Scoped Test (`/mode-test-rule`)

**When to use:**
- Adding test cases for a single rule
- Fixing test cases for a rule
- Creating comprehensive test suite for one rule

**What's unlocked:**
- ✅ `src/rules/cert_c/<CAT>/<RULE-ID>/tests/fail/*.c` (fail test cases)
- ✅ `src/rules/cert_c/<CAT>/<RULE-ID>/tests/pass/*.c` (pass test cases)

**What's locked:**
- ❌ All other test files (2,709 files from other rules)
- ❌ All implementations (283 rules)
- ❌ Utility files

**Example:**
```bash
# Architect
./scripts/claude_mode_test_rule.sh ARR38-C

# Claude
/mode-test-rule ARR38-C
# Can edit ARR38-C tests only, everything else locked
```

**Benefits:**
- Cannot accidentally modify other tests
- Safe to experiment with test cases
- Clear focus on one rule's test coverage

---

### 4. Broad Implementation Mode (`/mode-impl`)

**When to use:**
- Refactoring across multiple rules
- Making architectural changes
- Bulk updates to many rules
- Working on rule registry or integration

**What's unlocked:**
- ✅ All 283 rule implementations
- ✅ Utility files
- ✅ Module files (`mod.rs`, `integration.rs`)

**What's locked:**
- ❌ All test files (2,710 files)

**Example:**
```bash
# Architect
./scripts/claude_mode_impl.sh

# Claude
/mode-impl
# Can edit any implementation, tests are locked
```

**Benefits:**
- Work across multiple rules
- Make system-wide changes
- Refactor common patterns

**Caution:**
- High risk of unintended changes
- Use for deliberate multi-rule work only
- Review changes carefully

---

### 5. Broad Test Mode (`/mode-test`)

**When to use:**
- Bulk test updates (naming, structure)
- Test framework changes
- Adding tests for multiple rules
- Test data migration

**What's unlocked:**
- ✅ All 2,710 test files

**What's locked:**
- ❌ All implementations (283 rules)
- ❌ Utility files

**Example:**
```bash
# Architect
./scripts/claude_mode_test.sh

# Claude
/mode-test
# Can edit any tests, implementations are locked
```

**Benefits:**
- Bulk test operations
- Test framework migrations
- Cross-rule test patterns

**Caution:**
- High risk of breaking many tests
- Use for deliberate multi-rule test work only

---

## Mode Switching

### Mid-Session Mode Switch

You can switch modes during a session:

```bash
# Start with implementation
./scripts/claude_mode_impl_rule.sh ARR38-C

# Discover you need to work on tests
./scripts/claude_mode_test_rule.sh ARR38-C

# Now test files are unlocked, implementation is locked
```

Claude will auto-detect the new permissions.

### Reset to Default

To unlock everything (use with caution):

```bash
./scripts/claude_mode_reset.sh
# All files unlocked - dangerous!
```

## Best Practices

### DO:
- ✅ Start with narrowest scope possible (rule-scoped)
- ✅ Verify permissions before editing (`ls -l` the files)
- ✅ Switch modes explicitly when scope changes
- ✅ Use broad modes only for deliberate multi-rule work
- ✅ Reset permissions after session (cleanup)

### DON'T:
- ❌ Start with broad mode "just in case"
- ❌ Try to work around permission restrictions
- ❌ Leave non-default permissions active
- ❌ Use broad mode for single-rule work

## Troubleshooting

### "Permission denied" Error

This is **intentional and expected**. It means:
1. You're trying to edit a file outside your current scope
2. You need to ask architect to adjust permissions

**Steps:**
1. STOP - Don't try to work around it
2. Identify what you need to edit
3. Ask architect to run the appropriate mode script
4. Resume work once permissions are adjusted

### "Which mode should I use?"

Ask yourself:
- **How many rules am I touching?** → 1 rule = scoped mode, many = broad mode
- **Do I need utilities?** → Yes = `-utils`, No = regular
- **Am I working on code or tests?** → Code = `impl`, Tests = `test`

### Mode Verification

Claude should verify permissions at the start of every mode:

```bash
# In mode-impl-rule ARR38-C
ls -l src/rules/cert_c/ARR/ARR38-C/arr38_c.rs
# Should be: -rw-r--r-- (644, writable)

ls -l src/rules/cert_c/ARR/ARR30-C/arr30_c.rs
# Should be: -r--r--r-- (444, read-only)
```

If verification fails, architect needs to re-run the mode script.

## Examples

### Example 1: Implement ARR38-C (no utils needed)
```bash
Architect: ./scripts/claude_mode_impl_rule.sh ARR38-C
Claude: /mode-impl-rule ARR38-C
# Implement ARR38-C
# Commit changes
Architect: ./scripts/claude_mode_reset.sh
```

### Example 2: Implement ARR38-C + create utility function
```bash
Architect: ./scripts/claude_mode_impl_rule.sh ARR38-C
Claude: /mode-impl-rule ARR38-C
# Discover need for utility function
Claude: "I need to modify utilities, can you run claude_mode_impl_rule_utils.sh?"
Architect: ./scripts/claude_mode_impl_rule_utils.sh ARR38-C
Claude: Verified utilities unlocked, proceeding...
# Implement ARR38-C + utility
# Commit changes
Architect: ./scripts/claude_mode_reset.sh
```

### Example 3: Add tests for ARR38-C
```bash
Architect: ./scripts/claude_mode_test_rule.sh ARR38-C
Claude: /mode-test-rule ARR38-C
# Add test cases to tests/fail/ and tests/pass/
# cargo build (regenerate integration tests)
# cargo test ARR38-C
# Commit test files
Architect: ./scripts/claude_mode_reset.sh
```

### Example 4: Refactor error handling across all rules
```bash
Architect: ./scripts/claude_mode_impl.sh
Claude: /mode-impl
# Make changes to multiple rules
# Extensive testing
# Commit with clear description
Architect: ./scripts/claude_mode_reset.sh
```

## Safety Philosophy

**Principle: Narrow scope by default, broaden only when needed.**

- Start specific (rule-scoped)
- Escalate to broad mode only for multi-rule work
- Reset after session
- Permissions are safety net, not obstacle

**Why this works:**
- Prevents 90% of accidental edits
- Clear intent (which rule am I working on?)
- Easier code review (scope is obvious)
- Scales to 1,000+ rules without confusion

## Quick Reference

| Task | Command | Unlocks |
|------|---------|---------|
| Implement ARR38-C | `/mode-impl-rule ARR38-C` | ARR38-C impl only |
| Implement ARR38-C + utils | `/mode-impl-rule-utils ARR38-C` | ARR38-C impl + utilities |
| Add tests for ARR38-C | `/mode-test-rule ARR38-C` | ARR38-C tests only |
| Refactor many rules | `/mode-impl` | All impls |
| Update many tests | `/mode-test` | All tests |
| Unlock everything | `./scripts/claude_mode_reset.sh` | Everything (dangerous) |
