// FIO41-C: Compliant - no side effects in stream argument
#include <stdio.h>

void test_fio41c_pass() {
    FILE *stream = fopen("/tmp/file", "r");
    if (stream) {
        int c = getc(stream);  // OK: No side effects in argument
        fclose(stream);
    }
}
