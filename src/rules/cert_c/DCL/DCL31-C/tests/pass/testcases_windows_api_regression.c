/*
 * Rule: DCL31-C
 * Source: testcases
 * Status: PASS - Windows API functions are known standard functions
 * Regression: Round 9 fix — Windows APIs were flagged as undeclared
 */

#include <stdio.h>

void use_windows_apis(void) {
    CryptAcquireContext(NULL, NULL, NULL, 0, 0);
    CryptReleaseContext(0, 0);
    HeapAlloc(NULL, 0, 100);
    HeapFree(NULL, 0, NULL);
    CreateFileA("test", 0, 0, NULL, 0, 0, NULL);
    CloseHandle(NULL);
    RegOpenKeyExA(0, "test", 0, 0, NULL);
    RegCloseKey(0);
}
