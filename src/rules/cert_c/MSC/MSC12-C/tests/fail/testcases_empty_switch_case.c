/*
 * Rule: MSC12-C
 * Status: FAIL - Empty switch case (fallthrough without code)
 */

#include <stdio.h>

void f(int x) {
    switch (x) {
    case 1:
        /* empty case body — VIOLATION */
        break;
    case 2:
        printf("two\n");
        break;
    }
}
