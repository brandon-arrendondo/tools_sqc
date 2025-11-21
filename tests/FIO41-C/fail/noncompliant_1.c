// FIO41-C: Noncompliant - stream argument with side effects in getc/putc
#include <stdio.h>

FILE *get_stream(void);

void test_fio41c_fail() {
    int c = getc(get_stream());  // VIOLATION: Function call with side effects
}
