/*
 * Rule: DCL30-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL30-C violation
 */

#include <stdio.h>
 
const char *p;
void dont_do_this(void) {
  const char c_str[] = "This will change";
  p = c_str; /* Dangerous */
}

void innocuous(void) {
  printf("%s\n", p);
}

int main(void) {
  dont_do_this();
  innocuous();
  return 0;
}