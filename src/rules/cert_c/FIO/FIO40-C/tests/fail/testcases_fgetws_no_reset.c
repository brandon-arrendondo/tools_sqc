/*
 * Rule: FIO40-C
 * Source: testcases
 * Status: FAIL - Should trigger FIO40-C violation
 *
 * fgetws() failure without resetting wide string buffer
 */

#include <stdio.h>
#include <wchar.h>

void fgetws_no_reset(FILE *fp) {
    wchar_t wbuf[128];
    /* VIOLATION: failure branch does not reset wbuf */
    if (fgetws(wbuf, 128, fp) == NULL) {
        /* Handle error but don't reset buffer */
        fprintf(stderr, "fgetws failed\n");
    }
    wprintf(L"%ls\n", wbuf);
}
