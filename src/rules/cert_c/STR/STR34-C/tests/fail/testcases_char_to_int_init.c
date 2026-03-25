/*
 * Rule: STR34-C
 * Source: testcases
 * Status: FAIL - char-to-int via init_declarator without unsigned char cast
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
