/*
 * Rule: POS36-C
 * Source: testcases
 * Status: PASS - Should NOT trigger POS36-C violation
 *
 * setgid() called before setuid() (correct order)
 */

#include <unistd.h>

void drop_privs_correct_order(uid_t uid, gid_t gid) {
    /* COMPLIANT: setgid before setuid */
    setgid(gid);
    setuid(uid);
}
