/*
 * Rule: MEM30-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM30-C violation
 */

#include <stdlib.h>
 
struct node {
  int value;
  struct node *next;
};
 
void free_list(struct node *head) {
  struct node *q;
  for (struct node *p = head; p != NULL; p = q) {
    q = p->next;
    free(p);
  }
}