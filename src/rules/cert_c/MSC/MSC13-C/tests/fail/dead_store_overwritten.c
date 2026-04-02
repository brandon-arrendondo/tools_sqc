/*
 * Rule: MSC13-C
 * Status: FAIL - Dead store: value overwritten before being read
 */

#include <stdio.h>

void f(void) {
    int data = 'C';   /* VIOLATION: dead store — overwritten on next line */
    data = 'Z';
    printf("%c\n", data);
}
