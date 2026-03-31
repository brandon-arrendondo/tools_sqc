/*
 * Rule: WIN00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger WIN00-C violation
 *
 * LoadLibraryEx() with LOAD_LIBRARY_SEARCH flags
 */

#include <windows.h>

void load_plugin_safe(void) {
    /* COMPLIANT: explicit search path via flags */
    HMODULE lib = LoadLibraryEx("plugin.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (lib) {
        FreeLibrary(lib);
    }
}
