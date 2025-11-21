// FIO15-C: Compliant - ensure secure directory before file operations
#include <stdio.h>
#include <sys/stat.h>

int is_secure_directory(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        // Check if directory is secure (not world-writable, etc.)
        return !(st.st_mode & S_IWOTH);
    }
    return 0;
}

void test_fio15c_pass() {
    // OK: Check directory security before file operations
    if (is_secure_directory("/tmp")) {
        FILE *fp = fopen("/tmp/myfile", "w");
        if (fp) {
            fprintf(fp, "data");
            fclose(fp);
        }
    }
}
