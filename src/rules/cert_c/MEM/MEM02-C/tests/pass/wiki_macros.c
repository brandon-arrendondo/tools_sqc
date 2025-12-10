/*
 * Rule: MEM02-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM02-C violation
 * Description: Macro-based allocator with proper cast
 */

#include <stdlib.h>

#define MALLOC(type) ((type *)malloc(sizeof(type)))

typedef struct widget widget;
struct widget {
  char c[10];
  int i;
  double d;
};

void testcase_compliant_macro(void) {
    widget *p;
    p = MALLOC(widget);   /* Compliant: macro provides cast */
    if (p != NULL) {
        p->i = 0;
        p->d = 0.0;
    }
}
