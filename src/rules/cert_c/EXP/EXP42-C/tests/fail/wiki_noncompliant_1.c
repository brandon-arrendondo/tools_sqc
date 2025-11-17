/*
 * Rule: EXP42-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP42-C violation
 */

#include <string.h>
 
struct s {
  char c;
  int i;
  char buffer[13];
};
 
void compare(const struct s *left, const struct s *right) {  
  if ((left && right) &&
      (0 == memcmp(left, right, sizeof(struct s)))) {
    /* ... */
  }
}