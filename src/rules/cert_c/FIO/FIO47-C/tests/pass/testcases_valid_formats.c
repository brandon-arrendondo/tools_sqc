/*
 * Rule: FIO47-C
 * Source: testcases
 * Status: PASS - Valid format string patterns
 */

#include <stdio.h>

/* Correct basic format specifiers */
void correct_formats(void) {
    int i = 42;
    long l = 100L;
    unsigned u = 5;
    double d = 3.14;
    char c = 'x';
    const char *s = "hello";

    printf("%d\n", i);
    printf("%ld\n", l);
    printf("%u\n", u);
    printf("%f\n", d);
    printf("%c\n", c);
    printf("%s\n", s);
    printf("%p\n", (void *)s);
    printf("%%\n");
}

/* snprintf with correct format */
void safe_snprintf(char *buf, int size) {
    snprintf(buf, size, "%d items", 42);
}
