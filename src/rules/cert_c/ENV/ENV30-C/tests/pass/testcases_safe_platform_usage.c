/*
 * Rule: ENV30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger ENV30-C violation
 */

/*
 * CERT C ENV30-C Pass Case: safe_platform_usage.c
 *
 * This case demonstrates compliant usage across different platforms
 * and with platform-specific alternatives.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

/* COMPLIANT: Platform-safe environment variable access */
void safe_platform_getenv(void) {
    const char *var_name = "PATH";

#ifdef _WIN32
    /* Windows-specific safe approach */
    char *env_buffer = NULL;
    size_t env_size = 0;

    errno_t err = _dupenv_s(&env_buffer, &env_size, var_name);
    if (err == 0 && env_buffer != NULL) {
        printf("Windows safe PATH (first 100 chars): %.100s\n", env_buffer);
        free(env_buffer);  /* _dupenv_s allocates memory that we own */
    } else {
        printf("Windows: PATH not found or allocation failed\n");
    }
#else
    /* POSIX approach - immediate use or safe copying */
    const char *path_value = getenv(var_name);
    if (path_value != NULL) {
        printf("POSIX PATH (first 100 chars): %.100s\n", path_value);

        /* If we need to modify, create a copy */
        char *path_copy = strdup(path_value);
        if (path_copy != NULL) {
            printf("POSIX PATH copy length: %zu\n", strlen(path_copy));
            free(path_copy);
        }
    } else {
        printf("POSIX: PATH not found\n");
    }
#endif
}

/* COMPLIANT: Safe secure string functions (where available) */
void safe_string_operations_demo(void) {
    const char *home = getenv("HOME");

    if (home != NULL) {
        /* Calculate required buffer size */
        size_t home_len = strlen(home);
        size_t suffix_len = strlen("/documents");
        size_t total_size = home_len + suffix_len + 1;

        char *full_path = malloc(total_size);
        if (full_path != NULL) {
#ifdef _WIN32
            /* Windows secure string functions */
            strcpy_s(full_path, total_size, home);
            strcat_s(full_path, total_size, "/documents");
#else
            /* Standard C functions with manual bounds checking */
            strcpy(full_path, home);
            strcat(full_path, "/documents");
#endif
            printf("Safe document path: %s\n", full_path);
            free(full_path);
        }
    }
}

/* COMPLIANT: Safe locale handling across platforms */
void safe_cross_platform_locale(void) {
    /* Get current locale safely */
    const char *current_locale = setlocale(LC_ALL, NULL);

    if (current_locale != NULL) {
        /* Create a copy for analysis */
        char *locale_copy = malloc(strlen(current_locale) + 1);

        if (locale_copy != NULL) {
            strcpy(locale_copy, current_locale);

            printf("Platform locale analysis:\n");
            printf("  Current locale: %s\n", locale_copy);
            printf("  Locale length: %zu\n", strlen(locale_copy));

#ifdef _WIN32
            printf("  Platform: Windows\n");
#else
            printf("  Platform: POSIX\n");
#endif

            free(locale_copy);
        }
    }
}

/* COMPLIANT: Safe environment variable enumeration */
void safe_env_enumeration(void) {
    printf("Environment variable enumeration:\n");

    /* Platform-independent approach using known variables */
    const char *common_vars[] = {
#ifdef _WIN32
        "USERPROFILE", "USERNAME", "COMPUTERNAME", "TEMP", "TMP"
#else
        "HOME", "USER", "HOSTNAME", "TMPDIR", "SHELL"
#endif
    };

    int num_vars = sizeof(common_vars) / sizeof(common_vars[0]);

    for (int i = 0; i < num_vars; i++) {
        const char *value = getenv(common_vars[i]);

        if (value != NULL) {
            /* Safe display with length limiting */
            printf("  %s: %.80s%s\n",
                   common_vars[i],
                   value,
                   strlen(value) > 80 ? "..." : "");
        } else {
            printf("  %s: (not set)\n", common_vars[i]);
        }
    }
}

/* COMPLIANT: Safe path manipulation */
void safe_path_manipulation(void) {
    const char *base_path = getenv("HOME");
    if (base_path == NULL) {
#ifdef _WIN32
        base_path = getenv("USERPROFILE");
#else
        base_path = "/tmp";
#endif
    }

    if (base_path != NULL) {
        /* Safe path construction */
        const char *subdir = "myapp";
        const char *filename = "config.txt";

#ifdef _WIN32
        const char *separator = "\\";
#else
        const char *separator = "/";
#endif

        size_t total_len = strlen(base_path) + strlen(separator) +
                          strlen(subdir) + strlen(separator) +
                          strlen(filename) + 1;

        char *full_path = malloc(total_len);
        if (full_path != NULL) {
            snprintf(full_path, total_len, "%s%s%s%s%s",
                    base_path, separator, subdir, separator, filename);

            printf("Platform-appropriate path: %s\n", full_path);
            free(full_path);
        }
    }
}

/* COMPLIANT: Safe compiler and library detection */
void safe_compiler_detection(void) {
    printf("Compiler and library information:\n");

#ifdef __GNUC__
    printf("  Compiler: GCC %d.%d.%d\n",
           __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
    printf("  Compiler: MSVC %d\n", _MSC_VER);
#elif defined(__clang__)
    printf("  Compiler: Clang %d.%d.%d\n",
           __clang_major__, __clang_minor__, __clang_patchlevel__);
#else
    printf("  Compiler: Unknown\n");
#endif

#ifdef _WIN32
    printf("  Platform: Windows\n");
#elif defined(__linux__)
    printf("  Platform: Linux\n");
#elif defined(__APPLE__)
    printf("  Platform: macOS\n");
#elif defined(__unix__)
    printf("  Platform: Unix\n");
#else
    printf("  Platform: Unknown\n");
#endif

    /* Safe feature detection */
#ifdef _GNU_SOURCE
    printf("  GNU extensions available\n");
#endif

#ifdef _POSIX_C_SOURCE
    printf("  POSIX features available\n");
#endif
}

int main(void) {
    printf("=== ENV30-C Safe Platform Usage Demo ===\n");

    printf("\n1. Safe platform getenv:\n");
    safe_platform_getenv();

    printf("\n2. Safe string operations:\n");
    safe_string_operations_demo();

    printf("\n3. Safe cross-platform locale:\n");
    safe_cross_platform_locale();

    printf("\n4. Safe environment enumeration:\n");
    safe_env_enumeration();

    printf("\n5. Safe path manipulation:\n");
    safe_path_manipulation();

    printf("\n6. Safe compiler detection:\n");
    safe_compiler_detection();

    return 0;
}