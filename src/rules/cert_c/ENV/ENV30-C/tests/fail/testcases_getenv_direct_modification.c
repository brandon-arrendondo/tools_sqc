/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: getenv_direct_modification.c
 *
 * This case demonstrates violations where the return value of getenv()
 * is directly modified, leading to undefined behavior.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NON-COMPLIANT: Direct modification of getenv() return value */
void unsafe_path_modification(void) {
    char *path = getenv("PATH");

    if (path != NULL) {
        /* VIOLATION: Directly modifying the returned string */
        path[0] = 'X';  /* Undefined behavior */
        printf("Modified PATH: %s\n", path);
    }
}

/* NON-COMPLIANT: String operations on getenv() return value */
void unsafe_string_operations(void) {
    char *home_dir = getenv("HOME");

    if (home_dir != NULL) {
        /* VIOLATION: Using strcat on returned string */
        strcat(home_dir, "/documents");  /* Undefined behavior */
        printf("Documents path: %s\n", home_dir);
    }
}

/* NON-COMPLIANT: Character replacement in environment variable */
void unsafe_character_replacement(void) {
    char *user = getenv("USER");

    if (user != NULL) {
        /* VIOLATION: Replacing characters in-place */
        for (int i = 0; user[i] != '\0'; i++) {
            if (user[i] == 'a') {
                user[i] = 'A';  /* Undefined behavior */
            }
        }
        printf("Modified user: %s\n", user);
    }
}

/* NON-COMPLIANT: Using strcpy to overwrite returned buffer */
void unsafe_buffer_overwrite(void) {
    char *shell = getenv("SHELL");

    if (shell != NULL) {
        /* VIOLATION: Overwriting the entire buffer */
        strcpy(shell, "/bin/bash");  /* Undefined behavior */
        printf("New shell: %s\n", shell);
    }
}

/* NON-COMPLIANT: Appending to environment variable string */
void unsafe_string_append(void) {
    char *lang = getenv("LANG");

    if (lang != NULL) {
        /* VIOLATION: Appending suffix to returned string */
        strcat(lang, ".UTF-8");  /* Undefined behavior */
        printf("Modified language: %s\n", lang);
    }
}

/* NON-COMPLIANT: Memory operations on returned pointer */
void unsafe_memory_operations(void) {
    char *tmp_dir = getenv("TMPDIR");

    if (tmp_dir != NULL) {
        size_t len = strlen(tmp_dir);
        /* VIOLATION: Using memset on returned buffer */
        memset(tmp_dir, 'X', len);  /* Undefined behavior */
        printf("Cleared tmp dir: %s\n", tmp_dir);
    }
}

/* NON-COMPLIANT: Null terminator manipulation */
void unsafe_null_terminator(void) {
    char *display = getenv("DISPLAY");

    if (display != NULL && strlen(display) > 0) {
        /* VIOLATION: Modifying null terminator position */
        display[strlen(display) - 1] = '\0';  /* Undefined behavior */
        printf("Truncated display: %s\n", display);
    }
}

/* NON-COMPLIANT: Case conversion of environment variable */
void unsafe_case_conversion(void) {
    char *hostname = getenv("HOSTNAME");

    if (hostname != NULL) {
        /* VIOLATION: Converting to uppercase in-place */
        for (char *p = hostname; *p; p++) {
            if (*p >= 'a' && *p <= 'z') {
                *p = *p - 'a' + 'A';  /* Undefined behavior */
            }
        }
        printf("Uppercase hostname: %s\n", hostname);
    }
}

int main(void) {
    printf("=== ENV30-C getenv() Direct Modification Violations ===\n");

    /* Set some environment variables for testing */
    setenv("PATH", "/usr/bin:/bin", 1);
    setenv("HOME", "/home/user", 1);
    setenv("USER", "testuser", 1);
    setenv("SHELL", "/bin/sh", 1);
    setenv("LANG", "en_US", 1);
    setenv("TMPDIR", "/tmp", 1);
    setenv("DISPLAY", ":0.0", 1);
    setenv("HOSTNAME", "localhost", 1);

    printf("\n1. Unsafe path modification:\n");
    unsafe_path_modification();

    printf("\n2. Unsafe string operations:\n");
    unsafe_string_operations();

    printf("\n3. Unsafe character replacement:\n");
    unsafe_character_replacement();

    printf("\n4. Unsafe buffer overwrite:\n");
    unsafe_buffer_overwrite();

    printf("\n5. Unsafe string append:\n");
    unsafe_string_append();

    printf("\n6. Unsafe memory operations:\n");
    unsafe_memory_operations();

    printf("\n7. Unsafe null terminator:\n");
    unsafe_null_terminator();

    printf("\n8. Unsafe case conversion:\n");
    unsafe_case_conversion();

    return 0;
}