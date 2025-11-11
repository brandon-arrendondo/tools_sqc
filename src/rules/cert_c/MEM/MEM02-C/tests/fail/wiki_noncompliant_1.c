/*
 * Rule: MEM02-C
 * Source: wiki
 * Status: FAIL - Should trigger MEM02-C violation
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

widget *p;

/* ... */

p = malloc(sizeof(gadget)); /* Imminent problem */
if (p != NULL) {
  p->i = 0;                 /* Undefined behavior */
  p->d = 0.0;               /* Undefined behavior */
}