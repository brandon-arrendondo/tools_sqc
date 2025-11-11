/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Pass Case: safe_setlocale_usage.c
 *
 * This case demonstrates compliant usage of setlocale() by properly
 * handling return values without modification.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

/* COMPLIANT: Safe locale information retrieval */
void safe_get_locale_info(void) {
    /* Safe immediate use of setlocale return value */
    const char *current_locale = setlocale(LC_ALL, NULL);

    if (current_locale != NULL) {
        printf("Current locale: %s\n", current_locale);
    } else {
        printf("Unable to retrieve current locale\n");
    }
}

/* COMPLIANT: Safe locale copying for modification */
void safe_locale_copy_and_modify(void) {
    const char *numeric_locale = setlocale(LC_NUMERIC, NULL);

    if (numeric_locale != NULL) {
        /* Create a copy for safe modification */
        char *locale_copy = malloc(strlen(numeric_locale) + 20);

        if (locale_copy != NULL) {
            strcpy(locale_copy, numeric_locale);
            strcat(locale_copy, ".modified");
            printf("Modified locale copy: %s\n", locale_copy);
            free(locale_copy);
        }
    }
}

/* COMPLIANT: Safe locale setting and validation */
int safe_set_locale(int category, const char *locale_name) {
    /* Store original locale for restoration */
    const char *original = setlocale(category, NULL);
    char *original_copy = NULL;

    if (original != NULL) {
        original_copy = strdup(original);
        if (original_copy == NULL) {
            return -1;  /* Memory allocation failed */
        }
    }

    /* Attempt to set new locale */
    const char *result = setlocale(category, locale_name);

    if (result != NULL) {
        printf("Successfully set locale to: %s\n", result);
        free(original_copy);
        return 0;
    } else {
        printf("Failed to set locale to: %s\n", locale_name);

        /* Restore original locale */
        if (original_copy != NULL) {
            setlocale(category, original_copy);
            free(original_copy);
        }
        return -1;
    }
}

/* COMPLIANT: Safe locale comparison */
void safe_locale_comparison(void) {
    const char *ctype_locale = setlocale(LC_CTYPE, NULL);
    const char *time_locale = setlocale(LC_TIME, NULL);

    /* Safe to read and compare immediately */
    if (ctype_locale != NULL && time_locale != NULL) {
        if (strcmp(ctype_locale, time_locale) == 0) {
            printf("CTYPE and TIME locales are the same\n");
        } else {
            printf("CTYPE (%s) and TIME (%s) locales differ\n",
                   ctype_locale, time_locale);
        }
    }
}

/* COMPLIANT: Safe locale information extraction */
void safe_locale_info_extraction(void) {
    const char *all_locale = setlocale(LC_ALL, NULL);

    if (all_locale != NULL) {
        /* Create working copy for parsing */
        char *locale_work = strdup(all_locale);

        if (locale_work != NULL) {
            printf("Analyzing locale: %s\n", all_locale);

            /* Safe to parse the copy */
            char *token = strtok(locale_work, ";");
            int category = 0;
            const char *category_names[] = {
                "LC_CTYPE", "LC_NUMERIC", "LC_TIME",
                "LC_COLLATE", "LC_MONETARY", "LC_MESSAGES"
            };

            while (token != NULL && category < 6) {
                printf("  %s: %s\n", category_names[category], token);
                token = strtok(NULL, ";");
                category++;
            }

            free(locale_work);
        }
    }
}

/* COMPLIANT: Safe locale validation */
int safe_validate_locale(const char *locale_name) {
    if (locale_name == NULL) {
        return 0;
    }

    /* Test if locale is valid by trying to set it temporarily */
    const char *original = setlocale(LC_ALL, NULL);
    char *original_copy = NULL;

    if (original != NULL) {
        original_copy = strdup(original);
        if (original_copy == NULL) {
            return -1;
        }
    }

    /* Try to set the locale */
    const char *result = setlocale(LC_ALL, locale_name);
    int is_valid = (result != NULL);

    /* Restore original locale */
    if (original_copy != NULL) {
        setlocale(LC_ALL, original_copy);
        free(original_copy);
    }

    return is_valid;
}

/* COMPLIANT: Safe locale enumeration */
void safe_locale_enumeration(void) {
    /* Test common locales */
    const char *test_locales[] = {
        "C",
        "POSIX",
        "en_US.UTF-8",
        "en_GB.UTF-8",
        "de_DE.UTF-8",
        "fr_FR.UTF-8",
        NULL
    };

    printf("Testing locale availability:\n");
    for (int i = 0; test_locales[i] != NULL; i++) {
        int valid = safe_validate_locale(test_locales[i]);
        printf("  %s: %s\n", test_locales[i],
               valid > 0 ? "Available" :
               valid == 0 ? "Not available" : "Error");
    }
}

/* COMPLIANT: Safe monetary locale handling */
void safe_monetary_locale(void) {
    /* Save current monetary locale */
    const char *current = setlocale(LC_MONETARY, NULL);
    char *saved_locale = NULL;

    if (current != NULL) {
        saved_locale = strdup(current);
    }

    /* Try to set to C locale for monetary */
    const char *c_monetary = setlocale(LC_MONETARY, "C");
    if (c_monetary != NULL) {
        printf("Set monetary locale to C: %s\n", c_monetary);
    }

    /* Restore original locale */
    if (saved_locale != NULL) {
        setlocale(LC_MONETARY, saved_locale);
        printf("Restored monetary locale to: %s\n", saved_locale);
        free(saved_locale);
    }
}

/* COMPLIANT: Safe time locale handling */
void safe_time_locale(void) {
    const char *original_time = setlocale(LC_TIME, NULL);

    /* Use immediate result for display */
    printf("Original time locale: %s\n", original_time ?: "(null)");

    /* If we need to work with the string, copy it first */
    if (original_time != NULL) {
        size_t locale_len = strlen(original_time);
        char *analysis_buffer = malloc(locale_len + 50);

        if (analysis_buffer != NULL) {
            sprintf(analysis_buffer, "Time locale '%s' has %zu characters",
                    original_time, locale_len);
            printf("Analysis: %s\n", analysis_buffer);
            free(analysis_buffer);
        }
    }
}

int main(void) {
    printf("=== ENV30-C Safe setlocale() Usage Demo ===\n");

    printf("\n1. Safe locale information retrieval:\n");
    safe_get_locale_info();

    printf("\n2. Safe locale copy and modify:\n");
    safe_locale_copy_and_modify();

    printf("\n3. Safe locale setting:\n");
    safe_set_locale(LC_NUMERIC, "C");

    printf("\n4. Safe locale comparison:\n");
    safe_locale_comparison();

    printf("\n5. Safe locale information extraction:\n");
    safe_locale_info_extraction();

    printf("\n6. Safe locale enumeration:\n");
    safe_locale_enumeration();

    printf("\n7. Safe monetary locale handling:\n");
    safe_monetary_locale();

    printf("\n8. Safe time locale handling:\n");
    safe_time_locale();

    return 0;
}