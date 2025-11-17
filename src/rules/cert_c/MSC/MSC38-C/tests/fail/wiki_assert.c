/*
 * Rule: MSC38-C
 * Source: wiki
 * Status: FAIL - Should trigger MSC38-C violation
 */

#include <assert.h>
 
typedef void (*handler_type)(int);
 
void execute_handler(handler_type handler, int value) {
  handler(value);
}
 
void func(int e) {
  execute_handler(&(assert), e < 0);
}