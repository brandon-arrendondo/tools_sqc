// FIO44-C: Noncompliant - use arbitrary value with fsetpos
#include <stdio.h>

void test_fio44c_fail() {
    FILE *fp = fopen("/tmp/file", "r");
    if (fp) {
        fpos_t pos;
        pos.__pos = 100;  // VIOLATION: Arbitrary value not from fgetpos
        fsetpos(fp, &pos);
        fclose(fp);
    }
}
