/*
 * Rule: ERR07-C
 * Status: PASS - dlopen with hardcoded path (no taint)
 */

void *dlopen(const char *filename, int flags);

void f(void) {
    void *handle = dlopen("/usr/lib/libfoo.so", 1);  /* Safe: hardcoded path */
}
