// FIO38-C: Noncompliant - copy FILE object
#include <stdio.h>
#include <string.h>

void test_fio38c_fail() {
    FILE *fp1 = fopen("/tmp/file", "r");
    FILE fp2;
    if (fp1) {
        memcpy(&fp2, fp1, sizeof(FILE));  // VIOLATION: Copy FILE object
        fclose(fp1);
    }
}
