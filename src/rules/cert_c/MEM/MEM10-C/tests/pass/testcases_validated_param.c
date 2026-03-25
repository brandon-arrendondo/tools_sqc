/*
 * Rule: MEM10-C
 * Source: testcases
 * Status: PASS - Pointer validation via function or no direct check
 */

/* Validation via dedicated function */
int valid(void *ptr) {
    return ptr != NULL;
}

void incr_safe(int *intptr) {
    if (!valid(intptr)) {
        return;
    }
    (*intptr)++;
}

/* No NULL check at all (caller's responsibility) */
void just_use(int *ptr) {
    *ptr = 10;
}

/* No pointer parameters */
int add(int a, int b) {
    return a + b;
}
