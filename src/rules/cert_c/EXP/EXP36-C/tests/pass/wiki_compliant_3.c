/*
 * Rule: EXP36-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP36-C violation
 */

#include <string.h>
 
struct foo_header {
  int len;
  /* ... */
};
  
void func(char *data, size_t offset) {
  struct foo_header header; 
  memcpy(&header, data + offset, sizeof(header));

  /* ... */
}