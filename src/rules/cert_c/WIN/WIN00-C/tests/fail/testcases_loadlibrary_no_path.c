/*
 * Rule: WIN00-C
 * Source: testcases
 * Status: FAIL - Should trigger WIN00-C violation
 *
 * LoadLibrary() without explicit search path
 */

#include <windows.h>

void load_plugin(void) {
    /* VIOLATION: LoadLibrary without explicit search path */
    HMODULE lib = LoadLibrary("plugin.dll");
    if (lib) {
        FreeLibrary(lib);
    }
}
