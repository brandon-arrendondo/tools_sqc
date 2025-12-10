/*
 * Rule: MEM02-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM02-C violation
 * Description: Macro usage with proper cast inside macro
 */

#include <stdlib.h>

#define MALLOC(type) ((type *)malloc(sizeof(type)))

typedef struct widget widget;
struct widget {
  char c[10];
  int i;
  double d;
};

void testcase_compliant_macro_usage(void) {
    widget *p;

    /* ... */

    p = MALLOC(widget);   /* Compliant: macro casts */
    if (p != NULL) {
        p->i = 0;
        p->d = 0.0;
    }
}
