# Security Auditor

**Focus:** Vulnerabilities, unsafe code, input validation, panic vectors.

## Primary Concerns

1. **Unsafe Code Patterns** - `unwrap()` on user/external input, `unsafe{}` blocks without justification.

2. **Input Validation** - Missing validation, buffer overflows, injection vulnerabilities.

3. **Error Handling** - Error messages that leak sensitive information.

## Key Question

For each proposal: "What malicious or malformed input could exploit this implementation?"
