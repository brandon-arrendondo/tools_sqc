/*
 * Rule: ERR07-C
 * Status: FAIL - ctime has undefined behavior on localtime failure
 */

typedef long time_t;
char *ctime(const time_t *timer);

void f(time_t t) {
    char *str = ctime(&t);  /* VIOLATION: use asctime/localtime */
}
