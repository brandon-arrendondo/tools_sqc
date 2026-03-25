/*
 * Rule: API07-C
 * Source: testcases
 * Status: FAIL - Extended type confusion patterns
 */

#include <stdlib.h>

/* char assigned to void*, deref as long */
void confusion_char_to_long(void) {
    char c = 'X';
    void *data = &c;
    long val = *((long *)data);
    (void)val;
}

/* short to int confusion */
void confusion_short_to_int(void) {
    short s = 10;
    void *data = &s;
    int val = *((int *)data);
    (void)val;
}

/* float to long (different size) */
void confusion_float_to_long(void) {
    float f = 1.0f;
    void *data = &f;
    long val = *((long *)data);
    (void)val;
}

/* char to double (large size diff) */
void confusion_char_to_double(void) {
    char c = 'A';
    void *data = &c;
    double val = *((double *)data);
    (void)val;
}
