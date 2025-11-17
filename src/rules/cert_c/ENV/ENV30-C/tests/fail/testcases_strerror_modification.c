/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: strerror_modification.c
 *
 * This case demonstrates violations where the return value of strerror()
 * is modified, leading to undefined behavior.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

/* NON-COMPLIANT: Direct modification of strerror() return value */
void unsafe_error_message_modification(void) {
    /* Generate an error */
    int fd = open("/nonexistent/file", O_RDONLY);
    char *error_msg = strerror(errno);

    if (error_msg != NULL) {
        /* VIOLATION: Modifying the error message string */
        error_msg[0] = 'X';  /* Undefined behavior */
        printf("Modified error: %s\n", error_msg);
    }

    if (fd >= 0) close(fd);
}

/* NON-COMPLIANT: String concatenation on strerror() result */
void unsafe_error_message_append(void) {
    /* Trigger EACCES error */
    chmod("/etc/passwd", 0000);  /* This will likely fail with EPERM */
    int fd = open("/etc/passwd", O_WRONLY);
    char *error_msg = strerror(errno);

    if (error_msg != NULL) {
        /* VIOLATION: Appending to error message */
        strcat(error_msg, " - Custom suffix");  /* Undefined behavior */
        printf("Appended error: %s\n", error_msg);
    }

    if (fd >= 0) close(fd);
}

/* NON-COMPLIANT: Character replacement in error message */
void unsafe_error_message_replacement(void) {
    /* Generate EINVAL error */
    lseek(-1, 0, SEEK_SET);
    char *error_msg = strerror(errno);

    if (error_msg != NULL) {
        /* VIOLATION: Replacing spaces with underscores */
        for (char *p = error_msg; *p; p++) {
            if (*p == ' ') {
                *p = '_';  /* Undefined behavior */
            }
        }
        printf("Modified error message: %s\n", error_msg);
    }
}

/* NON-COMPLIANT: Overwriting error message with strcpy */
void unsafe_error_message_overwrite(void) {
    /* Generate EBADF error */
    close(-1);
    char *error_msg = strerror(errno);

    if (error_msg != NULL) {
        /* VIOLATION: Overwriting error message */
        strcpy(error_msg, "Custom error");  /* Undefined behavior */
        printf("Overwritten error: %s\n", error_msg);
    }
}

/* NON-COMPLIANT: Capitalizing error message */
void unsafe_error_message_capitalize(void) {
    /* Generate ENOENT error */
    open("/does/not/exist", O_RDONLY);
    char *error_msg = strerror(errno);

    if (error_msg != NULL && strlen(error_msg) > 0) {
        /* VIOLATION: Capitalizing first letter */
        if (error_msg[0] >= 'a' && error_msg[0] <= 'z') {
            error_msg[0] = error_msg[0] - 'a' + 'A';  /* Undefined behavior */
        }
        printf("Capitalized error: %s\n", error_msg);
    }
}

/* NON-COMPLIANT: Truncating error message */
void unsafe_error_message_truncate(void) {
    /* Generate ENOMEM-like error by setting errno */
    errno = ENOMEM;
    char *error_msg = strerror(errno);

    if (error_msg != NULL && strlen(error_msg) > 10) {
        /* VIOLATION: Truncating error message */
        error_msg[10] = '\0';  /* Undefined behavior */
        printf("Truncated error: %s\n", error_msg);
    }
}

/* NON-COMPLIANT: Adding prefix to error message */
void unsafe_error_message_prefix(void) {
    /* Generate EPERM error */
    setuid(0);  /* This will likely fail */
    char *error_msg = strerror(errno);

    if (error_msg != NULL) {
        /* VIOLATION: Using memmove to add prefix (still modifies returned buffer) */
        size_t msg_len = strlen(error_msg);
        const char *prefix = "ERROR: ";
        size_t prefix_len = strlen(prefix);

        memmove(error_msg + prefix_len, error_msg, msg_len + 1);  /* Undefined behavior */
        memcpy(error_msg, prefix, prefix_len);  /* Undefined behavior */

        printf("Prefixed error: %s\n", error_msg);
    }
}

/* NON-COMPLIANT: Multiple strerror calls with modification */
void unsafe_multiple_error_calls(void) {
    /* Generate first error */
    errno = EACCES;
    char *first_error = strerror(errno);

    /* Generate second error */
    errno = ENOENT;
    char *second_error = strerror(errno);

    /* VIOLATION: Modifying first error message after second call */
    /* Note: first_error might already be invalid due to static buffer reuse */
    if (first_error != NULL) {
        first_error[0] = 'F';  /* Undefined behavior */
        printf("Modified first error: %s\n", first_error);
    }

    /* VIOLATION: Modifying second error message */
    if (second_error != NULL) {
        strcat(second_error, "!");  /* Undefined behavior */
        printf("Modified second error: %s\n", second_error);
    }
}

/* NON-COMPLIANT: Using strtok on error message */
void unsafe_error_message_tokenize(void) {
    /* Generate a descriptive error */
    errno = EISDIR;
    char *error_msg = strerror(errno);

    if (error_msg != NULL) {
        /* VIOLATION: Using strtok which modifies the string */
        char *token = strtok(error_msg, " ");  /* Undefined behavior */
        while (token != NULL) {
            printf("Error token: %s\n", token);
            token = strtok(NULL, " ");
        }
    }
}

/* NON-COMPLIANT: Case conversion of error message */
void unsafe_error_message_case_convert(void) {
    /* Generate EBUSY error */
    errno = EBUSY;
    char *error_msg = strerror(errno);

    if (error_msg != NULL) {
        /* VIOLATION: Converting entire message to uppercase */
        for (char *p = error_msg; *p; p++) {
            if (*p >= 'a' && *p <= 'z') {
                *p = *p - 'a' + 'A';  /* Undefined behavior */
            }
        }
        printf("Uppercase error: %s\n", error_msg);
    }
}

int main(void) {
    printf("=== ENV30-C strerror() Modification Violations ===\n");

    printf("\n1. Unsafe error message modification:\n");
    unsafe_error_message_modification();

    printf("\n2. Unsafe error message append:\n");
    unsafe_error_message_append();

    printf("\n3. Unsafe error message replacement:\n");
    unsafe_error_message_replacement();

    printf("\n4. Unsafe error message overwrite:\n");
    unsafe_error_message_overwrite();

    printf("\n5. Unsafe error message capitalize:\n");
    unsafe_error_message_capitalize();

    printf("\n6. Unsafe error message truncate:\n");
    unsafe_error_message_truncate();

    printf("\n7. Unsafe error message prefix:\n");
    unsafe_error_message_prefix();

    printf("\n8. Unsafe multiple error calls:\n");
    unsafe_multiple_error_calls();

    printf("\n9. Unsafe error message tokenize:\n");
    unsafe_error_message_tokenize();

    printf("\n10. Unsafe error message case convert:\n");
    unsafe_error_message_case_convert();

    return 0;
}