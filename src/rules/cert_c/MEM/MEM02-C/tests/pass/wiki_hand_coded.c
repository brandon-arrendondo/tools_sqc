/*
 * Rule: MEM02-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM02-C violation
 * Description: malloc properly cast to correct type
 */

#include <stdlib.h>

typedef struct widget widget;
struct widget {
  char c[10];
  int i;
  double d;
};

void testcase_compliant_correct_cast(void) {
    widget *p;

    /* ... */

    p = (widget *)malloc(sizeof(widget)); /* Compliant: correct cast */
}
