# FIO34-C: Distinguish between characters read from a file and EOF or WEOF

## Rule Description

When reading characters from a file, it's critical to properly distinguish between valid character values and the end-of-file (EOF) indicator. The EOF macro expands to a negative integer constant (typically -1), but when character reading functions like `getc()` or `fgetc()` return this value, storing it in a `char` variable can lead to ambiguity and bugs.

## Key Requirements

### 1. Violation Patterns to Detect

- **Storing in char**: Using `char` or `unsigned char` variables to store the return value of character input functions
- **Direct EOF comparison**: Comparing characters read from a file directly to `EOF` or `WEOF` without proper type handling
- **Type truncation**: Immediately converting return values to smaller types before EOF comparison
- **Loop conditions**: Using char variables in loop conditions that check for EOF

### 2. Compliant Solutions

- Use `int` to store return values from `getc()`, `fgetc()`, `getchar()`
- Use `wint_t` for wide character functions like `getwc()`, `fgetwc()`, `getwchar()`
- Verify end-of-file using `feof()` and check for errors using `ferror()` after reading
- Check stream status after reading to confirm actual end-of-file condition

### 3. Affected Functions

**Standard character input functions:**
- `getc(FILE *stream)` - Returns int
- `fgetc(FILE *stream)` - Returns int
- `getchar(void)` - Returns int (equivalent to getc(stdin))
- `ungetc(int c, FILE *stream)` - Returns int

**Wide character input functions:**
- `getwc(FILE *stream)` - Returns wint_t
- `fgetwc(FILE *stream)` - Returns wint_t
- `getwchar(void)` - Returns wint_t

### 4. Common Violation Examples

**Noncompliant Example 1: char variable**
```c
char c;
while ((c = getc(file)) != EOF) {  // VIOLATION: char cannot reliably detect EOF
    // process character
}
```

**Noncompliant Example 2: Direct assignment to char**
```c
char buffer[256];
int i = 0;
char c = fgetc(file);  // VIOLATION: char loses EOF information
if (c != EOF) {        // This comparison is unreliable
    buffer[i++] = c;
}
```

**Compliant Example 1: Using int**
```c
int c;
while ((c = getc(file)) != EOF) {  // COMPLIANT: int preserves EOF
    // process character
    char ch = (char)c;  // Safe cast after EOF check
}
```

**Compliant Example 2: Checking stream status**
```c
int c;
while ((c = getc(file)) != EOF) {
    // process character
}
if (feof(file)) {
    // Handle end of file
} else if (ferror(file)) {
    // Handle error
}
```

## Technical Details

### Why This Is a Problem

1. **Sign Extension Issues**: On systems where `char` is signed and `EOF` is -1, a valid character with value 0xFF (255) will be sign-extended to -1 when stored in a char and compared to EOF.

2. **Platform Dependencies**: The signedness of `char` is implementation-defined. Code that works on one platform may fail on another.

3. **Data Loss**: Converting the int return value to char immediately loses the distinction between EOF and valid characters.

### Implementation Notes

- The rule should detect patterns where character input functions' return values are stored in `char` variables
- Loop conditions with embedded assignments need special attention
- Wide character functions need similar handling with `wint_t` instead of `wchar_t`

### Exceptions

Some functions that return EOF are safe to use directly:
- `fclose()` - Returns EOF on error
- `fflush()` - Returns EOF on error
- `fputs()` - Returns EOF on error
- `fputc()` - Returns EOF on error

These functions return EOF as an error indicator, not as a data value that could be confused with valid input.

## References

- [SEI CERT C Coding Standard - FIO34-C](https://wiki.sei.cmu.edu/confluence/display/c/FIO34-C.+Distinguish+between+characters+read+from+a+file+and+EOF+or+WEOF)
- ISO/IEC 9899:2011 Section 7.21.7, "Character input/output functions"