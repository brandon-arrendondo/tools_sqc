/*
 * Rule: STR34-C
 * Source: testcases
 * Status: PASS - Known limitation: init_declarator char-to-int not detected
 * TODO: Move to fail/ when init_declarator path is implemented (see PLAN.md)
 *
 * These are genuine STR34-C violations (char to int without unsigned char cast)
 * but the rule only checks assignment_expression and cast_expression, not init_declarator.
 */

/* Init declarator: char to int without cast */
void init_without_cast(const char *str) {
    int val = *str;
    (void)val;
}

/* Assignment from char array to int */
void array_to_int(const char buf[]) {
    int ch = buf[0];
    (void)ch;
}
