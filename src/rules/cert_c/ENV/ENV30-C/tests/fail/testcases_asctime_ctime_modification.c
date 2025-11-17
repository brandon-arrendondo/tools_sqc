/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: FAIL - Should trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Fail Case: asctime_ctime_modification.c
 *
 * This case demonstrates violations where the return values of asctime()
 * and ctime() are modified, leading to undefined behavior.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* NON-COMPLIANT: Direct modification of asctime() return value */
void unsafe_asctime_modification(void) {
    time_t current_time = time(NULL);
    struct tm *time_info = localtime(&current_time);
    char *time_string = asctime(time_info);

    if (time_string != NULL) {
        /* VIOLATION: Modifying the time string */
        time_string[0] = 'X';  /* Undefined behavior */
        printf("Modified asctime: %s", time_string);
    }
}

/* NON-COMPLIANT: String operations on asctime() result */
void unsafe_asctime_string_ops(void) {
    time_t current_time = time(NULL);
    struct tm *time_info = gmtime(&current_time);
    char *time_string = asctime(time_info);

    if (time_string != NULL) {
        /* VIOLATION: Removing newline character */
        size_t len = strlen(time_string);
        if (len > 0 && time_string[len - 1] == '\n') {
            time_string[len - 1] = '\0';  /* Undefined behavior */
        }
        printf("No newline asctime: %s\n", time_string);
    }
}

/* NON-COMPLIANT: Appending to asctime() result */
void unsafe_asctime_append(void) {
    time_t current_time = time(NULL);
    struct tm *time_info = localtime(&current_time);
    char *time_string = asctime(time_info);

    if (time_string != NULL) {
        /* VIOLATION: Appending timezone info */
        strcat(time_string, " UTC");  /* Undefined behavior */
        printf("Appended asctime: %s", time_string);
    }
}

/* NON-COMPLIANT: Direct modification of ctime() return value */
void unsafe_ctime_modification(void) {
    time_t current_time = time(NULL);
    char *time_string = ctime(&current_time);

    if (time_string != NULL) {
        /* VIOLATION: Replacing spaces with underscores */
        for (char *p = time_string; *p; p++) {
            if (*p == ' ') {
                *p = '_';  /* Undefined behavior */
            }
        }
        printf("Modified ctime: %s", time_string);
    }
}

/* NON-COMPLIANT: Overwriting ctime() result */
void unsafe_ctime_overwrite(void) {
    time_t current_time = time(NULL);
    char *time_string = ctime(&current_time);

    if (time_string != NULL) {
        /* VIOLATION: Overwriting with custom string */
        strcpy(time_string, "Custom Time String\n");  /* Undefined behavior */
        printf("Overwritten ctime: %s", time_string);
    }
}

/* NON-COMPLIANT: Character case conversion in time string */
void unsafe_time_case_conversion(void) {
    time_t current_time = time(NULL);
    char *time_string = ctime(&current_time);

    if (time_string != NULL) {
        /* VIOLATION: Converting to uppercase */
        for (char *p = time_string; *p; p++) {
            if (*p >= 'a' && *p <= 'z') {
                *p = *p - 'a' + 'A';  /* Undefined behavior */
            }
        }
        printf("Uppercase ctime: %s", time_string);
    }
}

/* NON-COMPLIANT: Truncating time string */
void unsafe_time_truncation(void) {
    time_t current_time = time(NULL);
    struct tm *time_info = localtime(&current_time);
    char *time_string = asctime(time_info);

    if (time_string != NULL && strlen(time_string) > 16) {
        /* VIOLATION: Truncating to show only date */
        time_string[16] = '\0';  /* Undefined behavior */
        printf("Truncated asctime: %s\n", time_string);
    }
}

/* NON-COMPLIANT: Replacing parts of time string */
void unsafe_time_replacement(void) {
    time_t current_time = time(NULL);
    char *time_string = ctime(&current_time);

    if (time_string != NULL) {
        /* VIOLATION: Replacing year with asterisks */
        /* Assuming format: "Day Mon DD HH:MM:SS YYYY\n" */
        size_t len = strlen(time_string);
        if (len >= 5) {
            /* Replace last 4 digits (year) with asterisks */
            for (int i = len - 6; i < len - 2; i++) {
                if (time_string[i] >= '0' && time_string[i] <= '9') {
                    time_string[i] = '*';  /* Undefined behavior */
                }
            }
        }
        printf("Masked year ctime: %s", time_string);
    }
}

/* NON-COMPLIANT: Multiple time function calls with modification */
void unsafe_multiple_time_calls(void) {
    time_t current_time = time(NULL);

    /* Get both asctime and ctime results */
    struct tm *time_info = localtime(&current_time);
    char *asc_time = asctime(time_info);
    char *c_time = ctime(&current_time);

    /* VIOLATION: Modifying asctime result after ctime call */
    /* Note: asc_time might already be invalid due to shared static buffer */
    if (asc_time != NULL) {
        asc_time[0] = 'A';  /* Undefined behavior */
        printf("Modified asctime: %s", asc_time);
    }

    /* VIOLATION: Modifying ctime result */
    if (c_time != NULL) {
        strcat(c_time, " MODIFIED");  /* Undefined behavior */
        printf("Modified ctime: %s", c_time);
    }
}

/* NON-COMPLIANT: Using strtok on time string */
void unsafe_time_tokenization(void) {
    time_t current_time = time(NULL);
    char *time_string = ctime(&current_time);

    if (time_string != NULL) {
        /* VIOLATION: Tokenizing time string */
        char *token = strtok(time_string, " ");  /* Undefined behavior */
        int token_count = 0;
        while (token != NULL && token_count < 3) {
            printf("Time token %d: %s\n", token_count, token);
            token = strtok(NULL, " ");
            token_count++;
        }
    }
}

/* NON-COMPLIANT: Custom formatting by modifying time string */
void unsafe_time_custom_format(void) {
    time_t current_time = time(NULL);
    struct tm *time_info = gmtime(&current_time);
    char *time_string = asctime(time_info);

    if (time_string != NULL) {
        /* VIOLATION: Custom formatting by direct modification */
        /* Try to change format from "Day Mon DD HH:MM:SS YYYY\n" to "DD/MM/YYYY" */
        if (strlen(time_string) >= 24) {
            /* This is a complex violation attempting to reformat in-place */
            memmove(time_string, time_string + 8, 2);  /* Move day */
            time_string[2] = '/';
            memmove(time_string + 3, time_string + 4, 3);  /* Move month */
            time_string[6] = '/';
            memmove(time_string + 7, time_string + 20, 4);  /* Move year */
            time_string[11] = '\0';
            printf("Custom format: %s\n", time_string);  /* Undefined behavior */
        }
    }
}

/* NON-COMPLIANT: Padding time string with extra characters */
void unsafe_time_padding(void) {
    time_t current_time = time(NULL);
    char *time_string = ctime(&current_time);

    if (time_string != NULL) {
        size_t len = strlen(time_string);
        /* VIOLATION: Adding padding at the end */
        if (len > 0 && time_string[len - 1] == '\n') {
            time_string[len - 1] = ' ';  /* Replace newline with space */
            time_string[len] = '*';      /* Add asterisk */
            time_string[len + 1] = '*';  /* Add another asterisk */
            time_string[len + 2] = '\n'; /* Add newline back */
            time_string[len + 3] = '\0'; /* Null terminate */
            printf("Padded ctime: %s", time_string);  /* Undefined behavior */
        }
    }
}

int main(void) {
    printf("=== ENV30-C asctime()/ctime() Modification Violations ===\n");

    printf("\n1. Unsafe asctime modification:\n");
    unsafe_asctime_modification();

    printf("\n2. Unsafe asctime string operations:\n");
    unsafe_asctime_string_ops();

    printf("\n3. Unsafe asctime append:\n");
    unsafe_asctime_append();

    printf("\n4. Unsafe ctime modification:\n");
    unsafe_ctime_modification();

    printf("\n5. Unsafe ctime overwrite:\n");
    unsafe_ctime_overwrite();

    printf("\n6. Unsafe time case conversion:\n");
    unsafe_time_case_conversion();

    printf("\n7. Unsafe time truncation:\n");
    unsafe_time_truncation();

    printf("\n8. Unsafe time replacement:\n");
    unsafe_time_replacement();

    printf("\n9. Unsafe multiple time calls:\n");
    unsafe_multiple_time_calls();

    printf("\n10. Unsafe time tokenization:\n");
    unsafe_time_tokenization();

    printf("\n11. Unsafe time custom format:\n");
    unsafe_time_custom_format();

    printf("\n12. Unsafe time padding:\n");
    unsafe_time_padding();

    return 0;
}