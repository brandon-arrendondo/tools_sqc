// FIO38-C: Compliant - don't copy FILE objects
#include <stdio.h>

void test_fio38c_pass() {
    FILE *fp1 = fopen("/tmp/file", "r");
    FILE *fp2 = fp1;  // OK: Just copy pointer, not FILE object itself
    if (fp1) {
        fclose(fp1);
    }
}
