/*
 * Rule: WIN30-C
 * Source: testcases
 * Status: PASS - Out of scope for WIN30-C (alloc/dealloc pairing).
 *         NULL security attributes is a separate concern not covered by this rule.
 */

/* CreateFile with NULL security attributes — not a WIN30-C violation */
void create_without_security(void) {
    CreateFileA("test.txt", 0x80000000, 0, NULL, 3, 0, NULL);
}

/* Pipe without security attributes — not a WIN30-C violation */
void pipe_without_security(void) {
    CreatePipe(NULL, NULL, NULL, 0);
}
