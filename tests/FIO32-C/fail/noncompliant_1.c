// FIO32-C: Noncompliant - device operations meant for files only
#include <stdio.h>

void test_fio32c_fail() {
    FILE *fp = fopen("/dev/tty", "r");  // Device file
    if (fp) {
        fseek(fp, 0, SEEK_END);  // VIOLATION: fseek on device
        fclose(fp);
    }
}
