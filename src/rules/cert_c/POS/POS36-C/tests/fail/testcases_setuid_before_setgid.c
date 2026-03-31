/*
 * Rule: POS36-C
 * Source: testcases
 * Status: FAIL - Should trigger POS36-C violation
 *
 * setuid() called before setgid() (wrong order)
 */

#include <unistd.h>

void drop_privs_wrong_order(uid_t uid, gid_t gid) {
    /* VIOLATION: must setgid before setuid */
    setuid(uid);
    setgid(gid);
}
