/*
 * Rule: ERR07-C (CWE-114)
 * Status: FAIL - Environment variable flows to dlopen
 */

void *dlopen(const char *filename, int flags);
char *getenv(const char *name);

void f(void) {
    char *lib_path = getenv("PLUGIN_PATH");  /* Taint source */
    void *handle = dlopen(lib_path, 1);      /* VIOLATION: tainted input to dlopen */
}
