/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: additional_violations_1.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: Compiler flag modification */
void unsafe_compiler_flags(void) {
    char *cflags = getenv("CFLAGS");
    if (cflags) {
        strcat(cflags, " -O2");  /* Undefined behavior */
        printf("Enhanced CFLAGS: %s\n", cflags);
    }
}

/* NON-COMPLIANT: Library path modification */
void unsafe_library_path(void) {
    char *ldpath = getenv("LD_LIBRARY_PATH");
    if (ldpath) {
        strcat(ldpath, ":/usr/local/lib");  /* Undefined behavior */
        printf("Enhanced LD_LIBRARY_PATH: %s\n", ldpath);
    }
}

/* NON-COMPLIANT: Python path modification */
void unsafe_python_path(void) {
    char *pypath = getenv("PYTHONPATH");
    if (pypath) {
        strcat(pypath, ":/opt/python/lib");  /* Undefined behavior */
        printf("Enhanced PYTHONPATH: %s\n", pypath);
    }
}

int main(void) {
    setenv("CFLAGS", "-Wall -Wextra", 1);
    setenv("LD_LIBRARY_PATH", "/usr/lib", 1);
    setenv("PYTHONPATH", "/usr/lib/python3", 1);

    unsafe_compiler_flags();
    unsafe_library_path();
    unsafe_python_path();
    return 0;
}