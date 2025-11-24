// FIO32-C: Compliant - check if file is regular before device-inappropriate ops
#include <stdio.h>
#include <sys/stat.h>

void test_fio32c_pass() {
    struct stat st;
    if (stat("/tmp/file", &st) == 0 && S_ISREG(st.st_mode)) {
        FILE *fp = fopen("/tmp/file", "r");
        if (fp) {
            fseek(fp, 0, SEEK_END);  // OK: Regular file
            fclose(fp);
        }
    }
}
