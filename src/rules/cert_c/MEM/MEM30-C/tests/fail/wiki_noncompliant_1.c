/*
 * Rule: MEM30-C
 * Source: wiki
 * Status: FAIL - Should trigger MEM30-C violation
 */

#include <stdlib.h>
 
struct node {
  int value;
  struct node *next;
};
 
void free_list(struct node *head) {
  for (struct node *p = head; p != NULL; p = p->next) {
    free(p);
  }
}