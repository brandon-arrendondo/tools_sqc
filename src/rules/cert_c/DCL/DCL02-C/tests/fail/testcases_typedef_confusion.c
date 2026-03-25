/*
 * Rule: DCL02-C
 * Source: testcases
 * Status: FAIL - Visually similar identifiers in declarations
 */

/* O/0 confusion in struct members */
struct Data {
    int field_O;
    int field_0;
};

/* l/1 confusion in parameters */
void process(int val1, int vall) {
    (void)val1;
    (void)vall;
}

/* S/5 confusion in local variables */
void compute(void) {
    int totalS = 0;
    int total5 = 0;
    (void)totalS;
    (void)total5;
}
