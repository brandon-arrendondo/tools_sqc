/*
 * Rule: API07-C
 * Source: testcases
 * Status: FAIL - Type confusion via void pointer
 */

#include <stdlib.h>

/* Cast-dereference from char to int (size mismatch) */
void type_confusion_char_to_int(void) {
    char c = 'A';
    void *data = &c;
    int val = *((int *)data);
    (void)val;
}

/* Cast-dereference from short to long (size mismatch) */
void type_confusion_short_to_long(void) {
    short s = 42;
    void *data = &s;
    long val = *((long *)data);
    (void)val;
}

/* Cast-dereference from float to double (size mismatch) */
void type_confusion_float_to_double(void) {
    float f = 3.14f;
    void *data = &f;
    double val = *((double *)data);
    (void)val;
}
