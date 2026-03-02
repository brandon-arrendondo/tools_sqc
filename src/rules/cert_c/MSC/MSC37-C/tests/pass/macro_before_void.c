/* Compliant: void function with macro prefix should not be flagged */
#define STATIC static

STATIC void my_debug(int level, const char *msg) {
    if (level > 0) {
        /* do nothing */
    }
}

STATIC void print_error(int code) {
    /* void function - no return needed */
}
