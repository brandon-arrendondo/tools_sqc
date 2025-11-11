/*
 * Rule: EXP42-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP42-C violation
 */

#include <string.h>
 
struct s {
  char c;
  int i;
  char buffer[13];
};
 
void compare(const struct s *left, const struct s *right) {  
  if ((left && right) &&
      (left->c == right->c) &&
      (left->i == right->i) &&
      (0 == memcmp(left->buffer, right->buffer, 13))) {
    /* ... */
  }
}