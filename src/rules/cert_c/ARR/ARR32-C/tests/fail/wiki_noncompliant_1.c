/*
 * Rule: ARR32-C
 * Source: wiki
 * Status: FAIL - Should trigger ARR32-C violation
 */

#include <stddef.h>

extern void do_work(int *array, size_t size);
 
void func(size_t size) {
  int vla[size];
  do_work(vla, size);
}