# INT32-C: Ensure that operations on signed integers do not result in overflow

## Rule Description
Signed integer overflow is undefined behavior in C. This rule aims to prevent integer operations that can result in overflow, particularly for operations involving:
- Integer operands in pointer arithmetic
- Array indexing
- Variable length array declarations
- Function arguments of type size_t or rsize_t

## Key Principles
1. Detect and prevent potential integer overflow scenarios
2. Validate integer operations before execution
3. Handle potential overflow conditions explicitly

## Non-Compliant Code Examples

### Addition Without Overflow Check
```c
void func(signed int si_a, signed int si_b) {
  signed int sum = si_a + si_b;  // Potential overflow
  /* ... */
}
```

### Multiplication Without Check
```c
void func(signed int si_a, signed int si_b) {
  signed int product = si_a * si_b;  // Potential overflow
  /* ... */
}
```

### Subtraction Without Check
```c
void func(signed int si_a, signed int si_b) {
  signed int diff = si_a - si_b;  // Potential overflow
  /* ... */
}
```

## Compliant Solutions

### Using stdckdint.h (C23)
```c
#include <stdckdint.h>

void f(signed int si_a, signed int si_b) {
  int sum;
  if (ckd_add(&sum, si_a, si_b)) {
    /* Handle error */
  }
  /* ... */
}
```

### Manual Overflow Checking (Addition)
```c
#include <limits.h>

void func(signed int si_a, signed int si_b) {
  if (((si_b > 0) && (si_a > (INT_MAX - si_b))) ||
      ((si_b < 0) && (si_a < (INT_MIN - si_b)))) {
    /* Handle error */
  } else {
    signed int sum = si_a + si_b;
    /* ... */
  }
}
```

### Manual Overflow Checking (Multiplication)
```c
#include <limits.h>

void func(signed int si_a, signed int si_b) {
  if (si_a > 0) {
    if (si_b > 0) {
      if (si_a > (INT_MAX / si_b)) {
        /* Handle error */
      }
    } else {
      if (si_b < (INT_MIN / si_a)) {
        /* Handle error */
      }
    }
  } else {
    if (si_b > 0) {
      if (si_a < (INT_MIN / si_b)) {
        /* Handle error */
      }
    } else {
      if ((si_a != 0) && (si_b < (INT_MAX / si_a))) {
        /* Handle error */
      }
    }
  }
  signed int product = si_a * si_b;
  /* ... */
}
```

## Risk Assessment
- **Severity**: High
- **Likelihood**: Likely
- **Potential Consequences**: Buffer overflows, arbitrary code execution

## Static Analysis Detection Points

### Arithmetic Operations to Check
1. **Addition**: `+` operator with signed integer operands
2. **Subtraction**: `-` operator with signed integer operands
3. **Multiplication**: `*` operator with signed integer operands
4. **Division**: `/` operator (check for INT_MIN / -1)
5. **Modulo**: `%` operator (check for INT_MIN % -1)
6. **Negation**: Unary `-` operator (check for -INT_MIN)

### Contexts to Analyze
1. Direct arithmetic expressions
2. Assignment operations with arithmetic
3. Function call arguments
4. Array indexing expressions
5. Pointer arithmetic
6. Loop increment/decrement operations
7. Compound assignment operators (`+=`, `-=`, `*=`, etc.)

### Detection Patterns
1. Look for binary arithmetic expressions with signed integer types
2. Check if operands are validated for overflow before operation
3. Identify missing overflow checks in critical contexts
4. Flag operations that could exceed INT_MAX or INT_MIN

## Implementation Notes for Static Analysis
- Focus on signed integer types: `int`, `signed int`, `short`, `long`, `long long`
- Consider macros and constants that might indicate limits checking
- Look for presence of `limits.h` include and limit constant usage
- Check for use of safe arithmetic libraries like `stdckdint.h`
- Consider data flow analysis to track potentially large values

## Automated Detection Tools
- Astrée
- CodeSonar
- Coverity
- Klocwork
- PVS-Studio
- TrustInSoft Analyzer

## References
- SEI CERT C Coding Standard: https://wiki.sei.cmu.edu/confluence/display/c/INT32-C
- C23 Standard: stdckdint.h checked integer arithmetic