/*
 * Rule: EXP40-C
 * Source: testcases
 * Status: FAIL - Removing const qualification through pointer assignments
 */

/* Pointer-to-pointer const bypass with file-scope declarations */
const int **g_ipp;
int *g_ip;

void const_ptr_ptr_bypass(void) {
    g_ipp = &g_ip;
}

/* Another const pointer-to-pointer bypass */
const char **g_cpp;
char *g_cp;

void const_char_ptr_ptr(void) {
    g_cpp = &g_cp;
}
