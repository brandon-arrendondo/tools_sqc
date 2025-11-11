/*
 * Rule: EXP36-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP36-C violation
 */

#include <string.h>
 
struct foo_header {
  int len;
  /* ... */
};
 
void func(char *data, size_t offset) {
  struct foo_header *tmp;
  struct foo_header header;

  tmp = (struct foo_header *)(data + offset);
  memcpy(&header, tmp, sizeof(header));

  /* ... */
}