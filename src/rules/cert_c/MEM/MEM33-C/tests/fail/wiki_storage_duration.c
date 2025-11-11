/*
 * Rule: MEM33-C
 * Source: wiki
 * Status: FAIL - Should trigger MEM33-C violation
 */

#include <stddef.h>
 
struct flex_array_struct {
  size_t num;
  int data[];
};
 
void func(void) {
  struct flex_array_struct flex_struct;
  size_t array_size = 4;

  /* Initialize structure */
  flex_struct.num = array_size;

  for (size_t i = 0; i < array_size; ++i) {
    flex_struct.data[i] = 0;
  }
}