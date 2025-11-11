/*
 * Rule: API00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger API00-C violation
 */

/*
 * CERT C API00-C Pass Case: validated_string_processing.c
 *
 * This case demonstrates compliant string processing functions
 * with comprehensive parameter validation and safe operations.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>

/* String processing result structure */
typedef struct {
    int success;
    size_t length;
    char *result;
    char error_message[128];
} StringResult;

/* COMPLIANT: Safe string duplication with validation */
char *safe_string_duplicate(const char *source) {
    /* Validate parameter */
    if (!source) {
        errno = EINVAL;
        return NULL;
    }

    size_t source_len = strlen(source);

    /* Check for reasonable length */
    const size_t MAX_STRING_LEN = 10 * 1024 * 1024;  /* 10 MB */
    if (source_len > MAX_STRING_LEN) {
        errno = ERANGE;
        return NULL;
    }

    /* Allocate memory */
    char *duplicate = malloc(source_len + 1);
    if (!duplicate) {
        errno = ENOMEM;
        return NULL;
    }

    /* Copy string */
    strcpy(duplicate, source);
    return duplicate;
}

/* COMPLIANT: Safe string concatenation with bounds checking */
StringResult safe_string_concat(const char *str1, const char *str2) {
    StringResult result = {0, 0, NULL, ""};

    /* Validate parameters */
    if (!str1 || !str2) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid parameters: one or both strings are NULL");
        return result;
    }

    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);

    /* Check for overflow */
    if (len1 > SIZE_MAX - len2 - 1) {
        errno = ERANGE;
        snprintf(result.error_message, sizeof(result.error_message),
                "String concatenation would overflow");
        return result;
    }

    size_t total_len = len1 + len2;

    /* Check for reasonable total length */
    const size_t MAX_CONCAT_LEN = 5 * 1024 * 1024;  /* 5 MB */
    if (total_len > MAX_CONCAT_LEN) {
        errno = ERANGE;
        snprintf(result.error_message, sizeof(result.error_message),
                "Concatenated string length %zu exceeds maximum %zu",
                total_len, MAX_CONCAT_LEN);
        return result;
    }

    /* Allocate memory */
    char *concatenated = malloc(total_len + 1);
    if (!concatenated) {
        errno = ENOMEM;
        snprintf(result.error_message, sizeof(result.error_message),
                "Cannot allocate %zu bytes", total_len + 1);
        return result;
    }

    /* Perform concatenation */
    strcpy(concatenated, str1);
    strcat(concatenated, str2);

    /* Success */
    result.success = 1;
    result.length = total_len;
    result.result = concatenated;
    snprintf(result.error_message, sizeof(result.error_message),
            "Successfully concatenated %zu characters", total_len);

    return result;
}

/* COMPLIANT: Safe string trimming with validation */
StringResult safe_string_trim(const char *source) {
    StringResult result = {0, 0, NULL, ""};

    /* Validate parameter */
    if (!source) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid parameter: source string is NULL");
        return result;
    }

    size_t source_len = strlen(source);

    /* Handle empty string */
    if (source_len == 0) {
        result.result = malloc(1);
        if (!result.result) {
            errno = ENOMEM;
            snprintf(result.error_message, sizeof(result.error_message),
                    "Cannot allocate memory for empty string");
            return result;
        }
        result.result[0] = '\0';
        result.success = 1;
        result.length = 0;
        strcpy(result.error_message, "Trimmed empty string");
        return result;
    }

    /* Find start of non-whitespace */
    const char *start = source;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    /* If all whitespace, return empty string */
    if (*start == '\0') {
        result.result = malloc(1);
        if (!result.result) {
            errno = ENOMEM;
            snprintf(result.error_message, sizeof(result.error_message),
                    "Cannot allocate memory for empty result");
            return result;
        }
        result.result[0] = '\0';
        result.success = 1;
        result.length = 0;
        strcpy(result.error_message, "Trimmed to empty string");
        return result;
    }

    /* Find end of non-whitespace */
    const char *end = source + source_len - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }

    /* Calculate trimmed length */
    size_t trimmed_len = (size_t)(end - start + 1);

    /* Allocate result string */
    result.result = malloc(trimmed_len + 1);
    if (!result.result) {
        errno = ENOMEM;
        snprintf(result.error_message, sizeof(result.error_message),
                "Cannot allocate %zu bytes", trimmed_len + 1);
        return result;
    }

    /* Copy trimmed content */
    strncpy(result.result, start, trimmed_len);
    result.result[trimmed_len] = '\0';

    /* Success */
    result.success = 1;
    result.length = trimmed_len;
    snprintf(result.error_message, sizeof(result.error_message),
            "Successfully trimmed to %zu characters", trimmed_len);

    return result;
}

/* COMPLIANT: Safe string replacement with validation */
StringResult safe_string_replace(const char *source, const char *search, const char *replace) {
    StringResult result = {0, 0, NULL, ""};

    /* Validate parameters */
    if (!source || !search || !replace) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid parameters: one or more strings are NULL");
        return result;
    }

    size_t search_len = strlen(search);
    if (search_len == 0) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Search string cannot be empty");
        return result;
    }

    size_t source_len = strlen(source);
    size_t replace_len = strlen(replace);

    /* Count occurrences */
    size_t count = 0;
    const char *pos = source;
    while ((pos = strstr(pos, search)) != NULL) {
        count++;
        pos += search_len;

        /* Prevent infinite loop and excessive replacements */
        if (count > 10000) {
            errno = ERANGE;
            snprintf(result.error_message, sizeof(result.error_message),
                    "Too many replacements (>10000)");
            return result;
        }
    }

    /* Calculate result length */
    size_t result_len;
    if (replace_len >= search_len) {
        /* Check for overflow */
        size_t size_diff = replace_len - search_len;
        if (count > (SIZE_MAX - source_len) / size_diff) {
            errno = ERANGE;
            snprintf(result.error_message, sizeof(result.error_message),
                    "Result string would be too large");
            return result;
        }
        result_len = source_len + count * size_diff;
    } else {
        size_t size_diff = search_len - replace_len;
        result_len = source_len - count * size_diff;
    }

    /* Check for reasonable result size */
    const size_t MAX_RESULT_LEN = 10 * 1024 * 1024;  /* 10 MB */
    if (result_len > MAX_RESULT_LEN) {
        errno = ERANGE;
        snprintf(result.error_message, sizeof(result.error_message),
                "Result length %zu exceeds maximum %zu", result_len, MAX_RESULT_LEN);
        return result;
    }

    /* Allocate result buffer */
    char *result_str = malloc(result_len + 1);
    if (!result_str) {
        errno = ENOMEM;
        snprintf(result.error_message, sizeof(result.error_message),
                "Cannot allocate %zu bytes", result_len + 1);
        return result;
    }

    /* Perform replacement */
    const char *src_pos = source;
    char *dst_pos = result_str;

    while ((pos = strstr(src_pos, search)) != NULL) {
        /* Copy text before match */
        size_t prefix_len = (size_t)(pos - src_pos);
        memcpy(dst_pos, src_pos, prefix_len);
        dst_pos += prefix_len;

        /* Copy replacement */
        memcpy(dst_pos, replace, replace_len);
        dst_pos += replace_len;

        /* Move past the search string */
        src_pos = pos + search_len;
    }

    /* Copy remaining text */
    strcpy(dst_pos, src_pos);

    /* Success */
    result.success = 1;
    result.length = result_len;
    result.result = result_str;
    snprintf(result.error_message, sizeof(result.error_message),
            "Successfully replaced %zu occurrences", count);

    return result;
}

/* COMPLIANT: Safe string to uppercase conversion */
StringResult safe_string_to_upper(const char *source) {
    StringResult result = {0, 0, NULL, ""};

    /* Validate parameter */
    if (!source) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid parameter: source string is NULL");
        return result;
    }

    size_t source_len = strlen(source);

    /* Allocate result buffer */
    char *upper_str = malloc(source_len + 1);
    if (!upper_str) {
        errno = ENOMEM;
        snprintf(result.error_message, sizeof(result.error_message),
                "Cannot allocate %zu bytes", source_len + 1);
        return result;
    }

    /* Convert to uppercase */
    for (size_t i = 0; i < source_len; i++) {
        upper_str[i] = (char)toupper((unsigned char)source[i]);
    }
    upper_str[source_len] = '\0';

    /* Success */
    result.success = 1;
    result.length = source_len;
    result.result = upper_str;
    snprintf(result.error_message, sizeof(result.error_message),
            "Successfully converted %zu characters to uppercase", source_len);

    return result;
}

/* COMPLIANT: Safe string split with validation */
int safe_string_split(const char *source, const char *delimiter, char ***tokens, size_t *token_count) {
    /* Validate parameters */
    if (!source || !delimiter || !tokens || !token_count) {
        errno = EINVAL;
        return -1;
    }

    if (strlen(delimiter) == 0) {
        errno = EINVAL;
        return -1;
    }

    /* Initialize outputs */
    *tokens = NULL;
    *token_count = 0;

    /* Handle empty source string */
    if (strlen(source) == 0) {
        return 0;  /* No tokens in empty string */
    }

    /* Create working copy of source */
    char *source_copy = safe_string_duplicate(source);
    if (!source_copy) {
        return -1;
    }

    /* Count tokens first */
    size_t count = 0;
    char *temp_copy = safe_string_duplicate(source);
    if (!temp_copy) {
        free(source_copy);
        return -1;
    }

    char *token = strtok(temp_copy, delimiter);
    while (token && count < 10000) {  /* Limit to prevent excessive memory use */
        count++;
        token = strtok(NULL, delimiter);
    }
    free(temp_copy);

    if (count == 0) {
        free(source_copy);
        return 0;  /* No tokens found */
    }

    if (count >= 10000) {
        free(source_copy);
        errno = ERANGE;
        return -1;  /* Too many tokens */
    }

    /* Allocate token array */
    char **token_array = malloc(count * sizeof(char *));
    if (!token_array) {
        free(source_copy);
        errno = ENOMEM;
        return -1;
    }

    /* Split and store tokens */
    size_t index = 0;
    token = strtok(source_copy, delimiter);
    while (token && index < count) {
        token_array[index] = safe_string_duplicate(token);
        if (!token_array[index]) {
            /* Clean up on allocation failure */
            for (size_t i = 0; i < index; i++) {
                free(token_array[i]);
            }
            free(token_array);
            free(source_copy);
            return -1;
        }
        index++;
        token = strtok(NULL, delimiter);
    }

    free(source_copy);

    /* Success - commit results */
    *tokens = token_array;
    *token_count = count;
    return 0;
}

/* COMPLIANT: Safe cleanup function for split results */
void safe_free_tokens(char **tokens, size_t token_count) {
    if (!tokens) {
        return;  /* Safe to call on NULL */
    }

    for (size_t i = 0; i < token_count; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

int main(void) {
    printf("=== Validated String Processing Demo ===\n\n");

    const char *test_string = "  Hello, World!  ";
    const char *search_replace_text = "The quick brown fox jumps over the lazy dog";

    /* Test string duplication */
    printf("1. String duplication:\n");
    char *dup = safe_string_duplicate(test_string);
    if (dup) {
        printf("   Original: '%s'\n", test_string);
        printf("   Duplicate: '%s'\n", dup);
        free(dup);
    } else {
        printf("   Failed: %s\n", strerror(errno));
    }

    /* Test string concatenation */
    printf("\n2. String concatenation:\n");
    StringResult concat_result = safe_string_concat("Hello, ", "World!");
    if (concat_result.success) {
        printf("   %s\n", concat_result.error_message);
        printf("   Result: '%s'\n", concat_result.result);
        free(concat_result.result);
    } else {
        printf("   Error: %s\n", concat_result.error_message);
    }

    /* Test string trimming */
    printf("\n3. String trimming:\n");
    StringResult trim_result = safe_string_trim(test_string);
    if (trim_result.success) {
        printf("   %s\n", trim_result.error_message);
        printf("   Original: '%s'\n", test_string);
        printf("   Trimmed: '%s'\n", trim_result.result);
        free(trim_result.result);
    } else {
        printf("   Error: %s\n", trim_result.error_message);
    }

    /* Test string replacement */
    printf("\n4. String replacement:\n");
    StringResult replace_result = safe_string_replace(search_replace_text, "o", "0");
    if (replace_result.success) {
        printf("   %s\n", replace_result.error_message);
        printf("   Original: '%s'\n", search_replace_text);
        printf("   Replaced: '%s'\n", replace_result.result);
        free(replace_result.result);
    } else {
        printf("   Error: %s\n", replace_result.error_message);
    }

    /* Test string case conversion */
    printf("\n5. String case conversion:\n");
    StringResult upper_result = safe_string_to_upper("Hello, World!");
    if (upper_result.success) {
        printf("   %s\n", upper_result.error_message);
        printf("   Uppercase: '%s'\n", upper_result.result);
        free(upper_result.result);
    } else {
        printf("   Error: %s\n", upper_result.error_message);
    }

    /* Test string splitting */
    printf("\n6. String splitting:\n");
    char **tokens;
    size_t token_count;
    if (safe_string_split("apple,banana,cherry,date", ",", &tokens, &token_count) == 0) {
        printf("   Split into %zu tokens:\n", token_count);
        for (size_t i = 0; i < token_count; i++) {
            printf("     [%zu]: '%s'\n", i, tokens[i]);
        }
        safe_free_tokens(tokens, token_count);
    } else {
        printf("   Split failed: %s\n", strerror(errno));
    }

    /* Test NULL parameter handling */
    printf("\n7. NULL parameter testing:\n");
    StringResult null_test = safe_string_concat(NULL, "test");
    if (!null_test.success) {
        printf("   Correctly rejected NULL parameter: %s\n", null_test.error_message);
    }

    printf("\n=== String processing demo completed ===\n");
    return 0;
}