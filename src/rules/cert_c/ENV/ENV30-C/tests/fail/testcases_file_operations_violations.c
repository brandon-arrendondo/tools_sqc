/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: file_operations_violations.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: File path modification */
void unsafe_file_path_modification(void) {
    char *home = getenv("HOME");
    if (home) {
        /* VIOLATION: Appending filename to home directory */
        strcat(home, "/.bashrc");  /* Undefined behavior */
        printf("Config file: %s\n", home);
    }
}

/* NON-COMPLIANT: Temporary directory modification */
void unsafe_temp_dir_modification(void) {
    char *tmpdir = getenv("TMPDIR");
    if (!tmpdir) tmpdir = getenv("TMP");
    if (tmpdir) {
        /* VIOLATION: Removing trailing slash */
        size_t len = strlen(tmpdir);
        if (len > 0 && tmpdir[len-1] == '/') {
            tmpdir[len-1] = '\0';  /* Undefined behavior */
        }
        printf("Clean tmpdir: %s\n", tmpdir);
    }
}

/* NON-COMPLIANT: Editor path modification */
void unsafe_editor_modification(void) {
    char *editor = getenv("EDITOR");
    if (editor) {
        /* VIOLATION: Adding command line options */
        strcat(editor, " -n");  /* Undefined behavior */
        printf("Editor with options: %s\n", editor);
    }
}

int main(void) {
    setenv("HOME", "/home/user", 1);
    setenv("TMPDIR", "/tmp/", 1);
    setenv("EDITOR", "nano", 1);

    unsafe_file_path_modification();
    unsafe_temp_dir_modification();
    unsafe_editor_modification();
    return 0;
}