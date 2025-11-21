// FIO44-C: Compliant - only use fsetpos with values from fgetpos
#include <stdio.h>

void test_fio44c_pass() {
    FILE *fp = fopen("/tmp/file", "r");
    if (fp) {
        fpos_t pos;
        fgetpos(fp, &pos);  // OK: Get position first
        fsetpos(fp, &pos);  // OK: Use value from fgetpos
        fclose(fp);
    }
}
