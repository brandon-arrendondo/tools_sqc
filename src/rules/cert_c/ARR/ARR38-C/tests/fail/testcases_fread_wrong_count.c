/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: FAIL
 * Reason: fread with element count not properly divided by element size
 */

#include <stddef.h>
#include <stdio.h>

void fread_incorrect(FILE *file) {
    enum { BUFFER_SIZE = 1024 };
    wchar_t wbuf[BUFFER_SIZE];
    const size_t size = sizeof(*wbuf);
    const size_t nitems = sizeof(wbuf);  // Should be sizeof(wbuf)/size

    // VIOLATION: nitems is total bytes, not element count
    size_t nread = fread(wbuf, size, nitems, file);  // Line 17 - VIOLATION
}

int main(void) {
    FILE *f = fopen("test.dat", "rb");
    if (f) {
        fread_incorrect(f);
        fclose(f);
    }
    return 0;
}
