/* Rule: FIO44-C
 * Source: testcases
 * Status: FAIL - fsetpos() called with value not from fgetpos()
 */

#include <stdio.h>
#include <string.h>

/* Case 1: fpos_t zeroed with memset, then used in fsetpos */
int test_memset_fpos(FILE *fp) {
    fpos_t pos;
    memset(&pos, 0, sizeof(pos));
    return fsetpos(fp, &pos);
}

/* Case 2: fpos_t declared but never initialized by fgetpos */
int test_uninitialized_fpos(FILE *fp) {
    fpos_t pos;
    /* No fgetpos call - pos is uninitialized */
    return fsetpos(fp, &pos);
}

/* Case 3: fpos_t initialized manually, not by fgetpos */
int test_manual_init_fpos(FILE *fp) {
    fpos_t pos;
    memset(&pos, 0xFF, sizeof(pos));
    return fsetpos(fp, &pos);
}
