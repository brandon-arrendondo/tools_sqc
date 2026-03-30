// Test: code after longjmp() is unreachable
#include <setjmp.h>
#include <stdio.h>

extern jmp_buf env;

void fail(void) {
    longjmp(env, 1);
    printf("unreachable\n");  // MSC07-C violation
}
