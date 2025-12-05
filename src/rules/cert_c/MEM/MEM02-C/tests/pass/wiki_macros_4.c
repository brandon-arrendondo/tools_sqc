/*
 * Rule: MEM02-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM02-C violation
 * Description: Using MALLOC_ARRAY macro
 */

#include <stdlib.h>

#define MALLOC_ARRAY(number, type) \
    ((type *)malloc((number) * sizeof(type)))

typedef struct widget widget;
struct widget {
  char c[10];
  int i;
  double d;
};

void testcase_compliant_macro_array_usage(void) {
    enum { N = 16 };
    widget *p;

    /* ... */

    p = MALLOC_ARRAY(N, widget);    /* Compliant */
}
