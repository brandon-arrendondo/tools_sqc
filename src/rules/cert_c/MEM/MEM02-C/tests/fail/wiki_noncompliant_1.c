/*
 * Rule: MEM02-C
 * Source: wiki
 * Status: FAIL - Should trigger MEM02-C violation
 * Description: malloc result not cast - type mismatch possible
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

void testcase_noncompliant_no_cast(void) {
    widget *p;

    /* ... */

    p = malloc(sizeof(gadget)); /* Violation: no cast, sizeof wrong type */
    if (p != NULL) {
        p->i = 0;
        p->d = 0.0;
    }
}
