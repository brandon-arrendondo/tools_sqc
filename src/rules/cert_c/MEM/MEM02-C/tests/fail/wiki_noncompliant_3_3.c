/*
 * Rule: MEM02-C
 * Source: wiki
 * Status: FAIL - Should trigger MEM02-C violation
 * Description: malloc cast to wrong type (gadget* assigned to widget*)
 */

#include <stdlib.h>

typedef struct gadget gadget;
struct gadget {
  int i;
  double d;
};

typedef struct widget widget;
struct widget {
  char c[10];
  int i;
  double d;
};

void testcase_noncompliant_wrong_cast(void) {
    widget *p;

    /* ... */

    p = (gadget *)malloc(sizeof(gadget)); /* Violation: cast to wrong type */
}
