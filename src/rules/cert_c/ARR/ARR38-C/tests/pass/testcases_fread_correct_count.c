/*
 * Rule: ARR38-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ARR38-C violation
 */

/*
 * Rule: ARR38-C - Guarantee that library functions do not form invalid pointers
 * Status: PASS
 * Reason: fread with properly calculated element count
 */

#include <stddef.h>
#include <stdio.h>

void fread_proper(FILE *file) {
    enum { BUFFER_SIZE = 1024 };
    wchar_t wbuf[BUFFER_SIZE];
    const size_t size = sizeof(*wbuf);

    // Properly divide total by element size - COMPLIANT
    const size_t nitems = sizeof(wbuf) / size;
    size_t nread = fread(wbuf, size, nitems, file);

    printf("Read %zu elements\n", nread);
}

int main(void) {
    FILE *f = fopen("test.dat", "rb");
    if (f) {
        fread_proper(f);
        fclose(f);
    }
    return 0;
}
