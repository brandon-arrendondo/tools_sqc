/*
 * Rule: EXP33-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP33-C violation
 */

/*
 * CERT C EXP33-C Fail Case: memory_copy_uninitialized.c
 */

#include <stdio.h>
#include <string.h>

/* NON-COMPLIANT: Memory operations with uninitialized data */
void unsafe_memory_ops(void) {
    char src[50];    /* Uninitialized source */
    char dest[50];   /* Uninitialized destination */

    memcpy(dest, src, 20);  /* Copying uninitialized data */
    printf("Copied data: %s\n", dest);
}

int main(void) {
    unsafe_memory_ops();
    return 0;
}