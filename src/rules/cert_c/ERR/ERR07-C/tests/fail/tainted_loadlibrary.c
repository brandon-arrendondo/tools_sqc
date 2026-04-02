/*
 * Rule: ERR07-C (CWE-114)
 * Status: FAIL - Untrusted input flows to LoadLibrary
 */

typedef void *HMODULE;
HMODULE LoadLibraryA(const char *lpLibFileName);
int recv(int s, char *buf, int len, int flags);

void f(int sock) {
    char path[256];
    recv(sock, path, sizeof(path), 0);  /* Taint source */
    HMODULE lib = LoadLibraryA(path);   /* VIOLATION: tainted input to LoadLibrary */
}
