/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Pass Case: safe_getenv_usage.c
 *
 * This case demonstrates compliant usage of getenv() by properly
 * copying return values before modification and using them safely.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* COMPLIANT: Safe duplication of environment variable */
char *safe_get_env_copy(const char *name) {
    const char *env_value = getenv(name);

    if (env_value == NULL) {
        return NULL;
    }

    /* Create a copy that can be safely modified */
    char *copy = malloc(strlen(env_value) + 1);
    if (copy == NULL) {
        return NULL;
    }

    strcpy(copy, env_value);
    return copy;  /* Caller must free this */
}

/* COMPLIANT: Safe path modification using copy */
void safe_path_modification(void) {
    char *path_copy = safe_get_env_copy("PATH");

    if (path_copy != NULL) {
        /* Safe to modify the copy */
        path_copy[0] = 'X';
        printf("Modified PATH copy: %s\n", path_copy);
        free(path_copy);
    } else {
        printf("PATH not found or allocation failed\n");
    }
}

/* COMPLIANT: Safe string operations on copied environment variable */
void safe_string_operations(void) {
    const char *home_dir = getenv("HOME");

    if (home_dir != NULL) {
        /* Calculate required size for concatenation */
        size_t total_size = strlen(home_dir) + strlen("/documents") + 1;
        char *documents_path = malloc(total_size);

        if (documents_path != NULL) {
            /* Safe operation on copied/new string */
            strcpy(documents_path, home_dir);
            strcat(documents_path, "/documents");
            printf("Documents path: %s\n", documents_path);
            free(documents_path);
        }
    }
}

/* COMPLIANT: Immediate use without modification */
void safe_immediate_use(void) {
    /* Safe to use return value immediately without storing or modifying */
    const char *user = getenv("USER");
    if (user != NULL) {
        printf("Current user: %s\n", user);
    }

    /* Safe to use in calculations */
    const char *debug_level = getenv("DEBUG_LEVEL");
    if (debug_level != NULL) {
        int level = atoi(debug_level);
        printf("Debug level: %d\n", level);
    }
}

/* COMPLIANT: Safe character replacement using copy */
void safe_character_replacement(void) {
    char *user_copy = safe_get_env_copy("USER");

    if (user_copy != NULL) {
        /* Safe to replace characters in the copy */
        for (char *p = user_copy; *p; p++) {
            if (*p == 'a') {
                *p = 'A';
            }
        }
        printf("Modified user copy: %s\n", user_copy);
        free(user_copy);
    }
}

/* COMPLIANT: Safe configuration file path creation */
void safe_config_path_creation(void) {
    const char *config_dir = getenv("CONFIG_DIR");
    const char *filename = "app.conf";

    if (config_dir != NULL) {
        /* Calculate size needed for full path */
        size_t path_size = strlen(config_dir) + strlen(filename) + 2; /* +2 for '/' and '\0' */
        char *config_path = malloc(path_size);

        if (config_path != NULL) {
            /* Build path in new buffer */
            snprintf(config_path, path_size, "%s/%s", config_dir, filename);
            printf("Config file path: %s\n", config_path);
            free(config_path);
        }
    }
}

/* COMPLIANT: Safe URL construction */
void safe_url_construction(void) {
    const char *base_url = getenv("API_URL");
    const char *endpoint = "/v1/users";

    if (base_url != NULL) {
        size_t url_size = strlen(base_url) + strlen(endpoint) + 1;
        char *full_url = malloc(url_size);

        if (full_url != NULL) {
            strcpy(full_url, base_url);
            strcat(full_url, endpoint);
            printf("Full API URL: %s\n", full_url);
            free(full_url);
        }
    }
}

/* COMPLIANT: Safe environment variable processing with strdup */
void safe_strdup_usage(void) {
    const char *lang = getenv("LANG");

    if (lang != NULL) {
        char *lang_copy = strdup(lang);  /* POSIX function for string duplication */

        if (lang_copy != NULL) {
            /* Safe to modify the duplicated string */
            char *dot = strchr(lang_copy, '.');
            if (dot != NULL) {
                *dot = '\0';  /* Remove encoding part */
            }
            printf("Language without encoding: %s\n", lang_copy);
            free(lang_copy);
        }
    }
}

/* COMPLIANT: Safe validation without modification */
void safe_validation(void) {
    const char *debug_flag = getenv("DEBUG");

    /* Safe to read and compare without modification */
    if (debug_flag != NULL) {
        if (strcmp(debug_flag, "true") == 0 || strcmp(debug_flag, "1") == 0) {
            printf("Debug mode is enabled\n");
        } else {
            printf("Debug mode is disabled\n");
        }
    }
}

/* COMPLIANT: Safe multiple environment variable handling */
void safe_multiple_env_vars(void) {
    /* Get multiple environment variables */
    const char *home = getenv("HOME");
    const char *user = getenv("USER");
    const char *shell = getenv("SHELL");

    /* Use them immediately without storing pointers */
    printf("User information:\n");
    printf("  Home: %s\n", home ?: "(not set)");
    printf("  User: %s\n", user ?: "(not set)");
    printf("  Shell: %s\n", shell ?: "(not set)");

    /* If we need copies for modification, create them */
    if (home != NULL && user != NULL) {
        size_t info_size = strlen(home) + strlen(user) + 50;
        char *user_info = malloc(info_size);

        if (user_info != NULL) {
            snprintf(user_info, info_size, "User %s lives in %s", user, home);
            printf("Summary: %s\n", user_info);
            free(user_info);
        }
    }
}

int main(void) {
    printf("=== ENV30-C Safe getenv() Usage Demo ===\n");

    /* Set up some environment variables for testing */
    setenv("PATH", "/usr/bin:/bin", 1);
    setenv("HOME", "/home/user", 1);
    setenv("USER", "testuser", 1);
    setenv("CONFIG_DIR", "/etc/myapp", 1);
    setenv("API_URL", "https://api.example.com", 1);
    setenv("LANG", "en_US.UTF-8", 1);
    setenv("DEBUG", "true", 1);
    setenv("SHELL", "/bin/bash", 1);
    setenv("DEBUG_LEVEL", "3", 1);

    printf("\n1. Safe path modification:\n");
    safe_path_modification();

    printf("\n2. Safe string operations:\n");
    safe_string_operations();

    printf("\n3. Safe immediate use:\n");
    safe_immediate_use();

    printf("\n4. Safe character replacement:\n");
    safe_character_replacement();

    printf("\n5. Safe config path creation:\n");
    safe_config_path_creation();

    printf("\n6. Safe URL construction:\n");
    safe_url_construction();

    printf("\n7. Safe strdup usage:\n");
    safe_strdup_usage();

    printf("\n8. Safe validation:\n");
    safe_validation();

    printf("\n9. Safe multiple environment variables:\n");
    safe_multiple_env_vars();

    return 0;
}