/*
 * Rule: DCL15-C
 * Source: testcases
 * Status: FAIL - File-scope variables without static should be declared static
 */

/* Global variable without static */
int global_counter = 0;

/* Global pointer without static */
char *global_buffer;

void use_globals(void) {
    global_counter++;
    global_buffer = "hello";
}
