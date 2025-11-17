/*
 * Rule: STR30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR30-C violation
 */

/*
 * Rule: STR30-C - Do not attempt to modify string literals
 * Status: PASS
 * Reason: Using const pointer for strrchr result on string literal
 */

#include <string.h>
#include <stddef.h>

char *get_dirname(const char *pathname, char *dirname, size_t size) {
    // Compliant: treating result as const when input is literal
    const char *slash;
    slash = strrchr(pathname, '/');
    if (slash) {
        ptrdiff_t slash_idx = slash - pathname;
        if ((size_t)slash_idx < size) {
            memcpy(dirname, pathname, slash_idx);
            dirname[slash_idx] = '\0';
            return dirname;
        }
    }
    return 0;
}

int main(void) {
    char buffer[100];
    get_dirname("/usr/local/bin", buffer, sizeof(buffer));
    return 0;
}
