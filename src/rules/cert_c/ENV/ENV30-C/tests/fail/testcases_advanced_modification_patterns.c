/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: advanced_modification_patterns.c
 *
 * This case demonstrates advanced violation patterns involving
 * complex modifications and edge cases.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <errno.h>

/* NON-COMPLIANT: Using sprintf to modify return value */
void unsafe_sprintf_modification(void) {
    char *env_user = getenv("USER");

    if (env_user != NULL) {
        /* VIOLATION: Using sprintf to overwrite the buffer */
        sprintf(env_user, "modified_%s", "user");  /* Undefined behavior */
        printf("Sprintf modified: %s\n", env_user);
    }
}

/* NON-COMPLIANT: Using memmove/memcpy on return value */
void unsafe_memory_functions(void) {
    char *locale = setlocale(LC_ALL, "C");

    if (locale != NULL && strlen(locale) >= 1) {
        /* VIOLATION: Using memmove to rearrange content */
        memmove(locale + 1, locale, strlen(locale));  /* Undefined behavior */
        locale[0] = 'X';
        printf("Memmove modified: %s\n", locale);
    }
}

/* NON-COMPLIANT: Pointer arithmetic with modification */
void unsafe_pointer_arithmetic(void) {
    char *error_msg = strerror(EINVAL);

    if (error_msg != NULL && strlen(error_msg) > 3) {
        /* VIOLATION: Using pointer arithmetic to modify */
        *(error_msg + 1) = 'X';  /* Undefined behavior */
        *(error_msg + 2) = 'Y';  /* Undefined behavior */
        printf("Pointer arithmetic modified: %s\n", error_msg);
    }
}

/* NON-COMPLIANT: Array subscript modification */
void unsafe_array_subscript(void) {
    char *path = getenv("PATH");

    if (path != NULL && strlen(path) > 5) {
        /* VIOLATION: Array-style modification */
        path[1] = 'A';  /* Undefined behavior */
        path[2] = 'B';  /* Undefined behavior */
        path[3] = 'C';  /* Undefined behavior */
        printf("Array subscript modified: %s\n", path);
    }
}

int main(void) {
    printf("=== ENV30-C Advanced Modification Patterns ===\n");

    /* Setup environment */
    setenv("USER", "testuser", 1);
    setenv("PATH", "/usr/bin:/bin", 1);

    printf("\n1. Unsafe sprintf modification:\n");
    unsafe_sprintf_modification();

    printf("\n2. Unsafe memory functions:\n");
    unsafe_memory_functions();

    printf("\n3. Unsafe pointer arithmetic:\n");
    unsafe_pointer_arithmetic();

    printf("\n4. Unsafe array subscript:\n");
    unsafe_array_subscript();

    return 0;
}