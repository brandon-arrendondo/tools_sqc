/*
 * Rule: WIN30-C
 * Source: testcases
 * Status: PASS - Known limitation: pattern not detected
 * TODO: Move to fail/ when implemented (see PLAN.md)
 */

/* CreateFile with NULL security attributes */
void create_without_security(void) {
    CreateFileA("test.txt", 0x80000000, 0, NULL, 3, 0, NULL);
}

/* Pipe without security attributes */
void pipe_without_security(void) {
    CreatePipe(NULL, NULL, NULL, 0);
}
