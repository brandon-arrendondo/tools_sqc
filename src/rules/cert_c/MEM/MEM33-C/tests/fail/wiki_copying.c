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
 
void func(struct flex_array_struct *struct_a,
          struct flex_array_struct *struct_b) {
  *struct_b = *struct_a;
}