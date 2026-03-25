/* Rule: FIO44-C
 * Source: testcases
 * Status: PASS - fsetpos() called with value obtained from fgetpos()
 */

#include <stdio.h>

/* Case 1: fgetpos then fsetpos (correct usage) */
int test_proper_fsetpos(FILE *fp) {
    fpos_t pos;
    if (fgetpos(fp, &pos) != 0) {
        return -1;
    }
    /* Read some data ... */
    return fsetpos(fp, &pos);
}

/* Case 2: Save and restore position around read */
int test_save_restore(FILE *fp) {
    fpos_t saved;
    int rc;
    char buf[256];

    rc = fgetpos(fp, &saved);
    if (rc != 0) return rc;

    fgets(buf, sizeof(buf), fp);

    rc = fsetpos(fp, &saved);
    return rc;
}

/* Case 3: Multiple save/restore cycles */
int test_multiple_saves(FILE *fp) {
    fpos_t pos1;
    fpos_t pos2;

    fgetpos(fp, &pos1);
    /* read some data */
    fgetpos(fp, &pos2);
    /* read more data */
    fsetpos(fp, &pos1);
    /* re-read first block */
    fsetpos(fp, &pos2);
    return 0;
}

/* Case 4: No fsetpos call at all (no violation possible) */
void test_no_fsetpos(FILE *fp) {
    fpos_t pos;
    fgetpos(fp, &pos);
    /* Just saved position, never restored */
}
