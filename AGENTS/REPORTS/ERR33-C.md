# ERR33-C: Detect and handle standard library errors

## Rule Description
The majority of standard library functions return either a valid value or a value indicating an error. Failing to check return values can lead to unexpected or undefined behavior.

## Key Principles
- Detect and handle errors for standard library functions
- Check return values for potential failure conditions
- Implement appropriate error handling strategies

## Non-Compliant Code Examples

### Memory Allocation Functions
```c
// calloc() not checked
void *ptr = calloc(10, sizeof(int));
*ptr = 42;  // Potential null pointer dereference
```

### Memory Reallocation
```c
// realloc() not checked properly
int *ptr = malloc(10 * sizeof(int));
ptr = realloc(ptr, 20 * sizeof(int));  // Loses original ptr on failure
```

### File Operations
```c
// fseek() return value ignored
FILE *file = fopen("data.txt", "r");
fseek(file, 100, SEEK_SET);  // Could fail
int value = fgetc(file);  // Position uncertain
```

### String/Locale Functions
```c
// setlocale() failure not checked
setlocale(LC_ALL, "");  // Could fail to set locale
```

### Formatted Output Functions
```c
// snprintf() return value ignored
char buffer[10];
snprintf(buffer, sizeof(buffer), "%s", very_long_string);  // Could truncate
```

## Compliant Solutions

### Memory Allocation with Error Checking
```c
void *ptr = calloc(10, sizeof(int));
if (ptr == NULL) {
    /* Handle allocation failure */
    return -1;
}
*ptr = 42;  // Safe to use
```

### Proper Memory Reallocation
```c
int *ptr = malloc(10 * sizeof(int));
int *new_ptr = realloc(ptr, 20 * sizeof(int));
if (new_ptr == NULL) {
    /* Handle reallocation failure, ptr still valid */
    free(ptr);
    return -1;
}
ptr = new_ptr;  // Success
```

### File Operations with Error Checking
```c
FILE *file = fopen("data.txt", "r");
if (file == NULL) {
    /* Handle file open failure */
    return -1;
}
if (fseek(file, 100, SEEK_SET) != 0) {
    /* Handle seek failure */
    fclose(file);
    return -1;
}
int value = fgetc(file);
```

### String/Locale Functions with Validation
```c
if (setlocale(LC_ALL, "") == NULL) {
    /* Handle locale setting failure */
    return -1;
}
```

### Formatted Output with Length Checking
```c
char buffer[10];
int result = snprintf(buffer, sizeof(buffer), "%s", string);
if (result < 0 || result >= sizeof(buffer)) {
    /* Handle formatting error or truncation */
    return -1;
}
```

## Functions That Must Be Checked

### Memory Management Functions
- `malloc()` - Returns NULL on failure
- `calloc()` - Returns NULL on failure
- `realloc()` - Returns NULL on failure (original ptr remains valid)
- `aligned_alloc()` - Returns NULL on failure

### File I/O Functions
- `fopen()` - Returns NULL on failure
- `freopen()` - Returns NULL on failure
- `fseek()` - Returns non-zero on failure
- `ftell()` - Returns -1L on failure
- `fsetpos()` - Returns non-zero on failure
- `fgetpos()` - Returns non-zero on failure
- `fread()` - Check return value vs expected
- `fwrite()` - Check return value vs expected
- `fflush()` - Returns EOF on failure
- `fclose()` - Returns EOF on failure

### String/Character Functions
- `setlocale()` - Returns NULL on failure
- `strtol()`, `strtod()`, etc. - Check errno and endptr
- `strftime()` - Returns 0 on failure
- `mbstowcs()` - Returns (size_t)-1 on failure
- `wcstombs()` - Returns (size_t)-1 on failure

### Formatted I/O Functions
- `printf()` family - Returns negative on failure
- `scanf()` family - Returns EOF on failure
- `snprintf()` - Check for truncation/failure

### Time Functions
- `time()` - Returns (time_t)-1 on failure
- `mktime()` - Returns (time_t)-1 on failure
- `clock()` - Returns (clock_t)-1 on failure

### System Functions
- `system()` - Returns -1 on failure
- `getenv()` - Returns NULL if not found (may be valid)
- `atexit()` - Returns non-zero on failure
- `signal()` - Returns SIG_ERR on failure

## Functions That Can Be Safely Ignored

### Output Functions (Often Safe to Ignore)
- `putchar()` - Failure rarely critical
- `puts()` - Failure rarely critical
- `putc()` - Failure rarely critical

### Memory Copy Functions (Cannot Fail)
- `memcpy()` - Cannot fail with valid arguments
- `memset()` - Cannot fail with valid arguments
- `memmove()` - Cannot fail with valid arguments
- `strcpy()` - Cannot fail with valid arguments (but has other issues)

## Risk Assessment
- **Severity**: High
- **Likelihood**: Likely
- **Potential Consequences**:
  - Unpredictable program behavior
  - Potential denial-of-service attacks
  - Possible arbitrary code execution vulnerabilities
  - Resource leaks
  - Data corruption

## Static Analysis Detection Points

### Function Call Patterns to Check
1. **Ignored Return Values**: Function calls where return value is not assigned or checked
2. **Unused Variables**: Return values assigned but never checked
3. **Missing Null Checks**: Pointers used without null validation
4. **Error Code Comparison**: Return values not compared against failure indicators

### Detection Strategies
1. Track function calls that return error indicators
2. Analyze control flow to ensure error checking
3. Look for immediate use of potentially failed results
4. Check for error handling patterns (if statements, goto error handling)

### Context Analysis
1. **Assignment Context**: `ptr = malloc(size)` without subsequent null check
2. **Conditional Context**: Missing `if (ptr == NULL)` after allocation
3. **Usage Context**: Using return value without validation
4. **Error Propagation**: Functions that should propagate errors upward

## Implementation Notes for Static Analysis
- Focus on functions with well-defined error return values
- Consider immediate context of function calls
- Look for error handling patterns in surrounding code
- Track data flow from function return to usage
- Consider exceptions for functions where errors can be safely ignored
- Analyze control flow to detect proper error handling

## Automated Detection Tools
- Coverity
- CodeSonar
- Klocwork
- Parasoft C/C++test
- PC-lint/PC-lint Plus
- Clang Static Analyzer

## References
- SEI CERT C Coding Standard: https://wiki.sei.cmu.edu/confluence/display/c/ERR33-C
- C Standard Library Error Handling Patterns
- POSIX Error Handling Guidelines