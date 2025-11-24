// FIO15-C: Noncompliant - file operations in insecure directory
#include <stdio.h>

void test_fio15c_fail() {
    // VIOLATION: File operations in world-writable directory without checks
    FILE *fp = fopen("/tmp/myfile", "w");
    if (fp) {
        fprintf(fp, "data");
        fclose(fp);
    }
}
