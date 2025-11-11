/*
 * Rule: API00-C
 * Source: testcases
 * Status: PASS - Should NOT trigger API00-C violation
 */

/*
 * CERT C API00-C Pass Case: defensive_parsing_functions.c
 *
 * This case demonstrates compliant parsing functions with
 * comprehensive parameter validation and safe processing.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>

/* Parsing result structure */
typedef struct {
    int success;
    char *result;
    size_t result_length;
    char error_message[128];
} ParseResult;

/* Configuration entry structure */
typedef struct {
    char *key;
    char *value;
} ConfigEntry;

/* COMPLIANT: Safe integer parsing with comprehensive validation */
int safe_parse_integer(const char *str, long *result, int base) {
    /* Validate parameters */
    if (!str || !result) {
        errno = EINVAL;
        return -1;
    }

    /* Validate base parameter */
    if (base != 0 && (base < 2 || base > 36)) {
        errno = EINVAL;
        return -1;
    }

    /* Check for empty string */
    if (strlen(str) == 0) {
        errno = EINVAL;
        return -1;
    }

    /* Check string length is reasonable */
    const size_t MAX_NUMBER_LEN = 32;
    if (strlen(str) > MAX_NUMBER_LEN) {
        errno = ERANGE;
        return -1;
    }

    /* Validate that string contains only valid characters for the base */
    const char *valid_chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    size_t max_chars = (base == 0) ? 36 : (size_t)base;

    for (const char *p = str; *p; p++) {
        if (*p == '+' || *p == '-') {
            if (p != str) {  /* Sign only allowed at start */
                errno = EINVAL;
                return -1;
            }
            continue;
        }

        if (*p == 'x' || *p == 'X') {
            if (base != 0 && base != 16) {
                errno = EINVAL;
                return -1;
            }
            continue;
        }

        int char_found = 0;
        for (size_t i = 0; i < max_chars && valid_chars[i]; i++) {
            if (tolower(*p) == tolower(valid_chars[i])) {
                char_found = 1;
                break;
            }
        }

        if (!char_found) {
            errno = EINVAL;
            return -1;
        }
    }

    /* Use strtol for safe conversion */
    char *endptr;
    errno = 0;  /* Reset errno before conversion */
    long value = strtol(str, &endptr, base);

    /* Check for conversion errors */
    if (errno == ERANGE) {
        return -1;  /* Overflow or underflow */
    }

    if (endptr == str) {
        errno = EINVAL;
        return -1;  /* No conversion performed */
    }

    if (*endptr != '\0') {
        errno = EINVAL;
        return -1;  /* Extra characters after number */
    }

    *result = value;
    return 0;
}

/* COMPLIANT: Safe floating-point parsing with validation */
int safe_parse_double(const char *str, double *result) {
    /* Validate parameters */
    if (!str || !result) {
        errno = EINVAL;
        return -1;
    }

    /* Check for empty string */
    if (strlen(str) == 0) {
        errno = EINVAL;
        return -1;
    }

    /* Check string length is reasonable */
    const size_t MAX_FLOAT_LEN = 64;
    if (strlen(str) > MAX_FLOAT_LEN) {
        errno = ERANGE;
        return -1;
    }

    /* Validate characters (basic check for floating-point format) */
    int has_digit = 0;
    int has_decimal = 0;
    int has_exponent = 0;

    for (const char *p = str; *p; p++) {
        if (isdigit(*p)) {
            has_digit = 1;
        } else if (*p == '.') {
            if (has_decimal || has_exponent) {
                errno = EINVAL;
                return -1;  /* Multiple decimal points or decimal after exponent */
            }
            has_decimal = 1;
        } else if (*p == 'e' || *p == 'E') {
            if (has_exponent || !has_digit) {
                errno = EINVAL;
                return -1;  /* Multiple exponents or exponent without preceding digit */
            }
            has_exponent = 1;
        } else if (*p == '+' || *p == '-') {
            if (p != str && *(p-1) != 'e' && *(p-1) != 'E') {
                errno = EINVAL;
                return -1;  /* Sign only allowed at start or after exponent */
            }
        } else {
            errno = EINVAL;
            return -1;  /* Invalid character */
        }
    }

    if (!has_digit) {
        errno = EINVAL;
        return -1;  /* No digits found */
    }

    /* Use strtod for safe conversion */
    char *endptr;
    errno = 0;  /* Reset errno before conversion */
    double value = strtod(str, &endptr);

    /* Check for conversion errors */
    if (errno == ERANGE) {
        return -1;  /* Overflow or underflow */
    }

    if (endptr == str) {
        errno = EINVAL;
        return -1;  /* No conversion performed */
    }

    if (*endptr != '\0') {
        errno = EINVAL;
        return -1;  /* Extra characters after number */
    }

    /* Check for NaN or infinity */
    if (!isfinite(value)) {
        errno = ERANGE;
        return -1;
    }

    *result = value;
    return 0;
}

/* COMPLIANT: Safe CSV line parsing with validation */
ParseResult safe_parse_csv_line(const char *line, char delimiter) {
    ParseResult result = {0, NULL, 0, ""};

    /* Validate parameters */
    if (!line) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "NULL line parameter");
        return result;
    }

    /* Validate delimiter is printable and not a quote */
    if (!isprint(delimiter) || delimiter == '"' || delimiter == '\'') {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid delimiter character");
        return result;
    }

    size_t line_len = strlen(line);

    /* Check for reasonable line length */
    const size_t MAX_CSV_LINE_LEN = 100 * 1024;  /* 100 KB */
    if (line_len > MAX_CSV_LINE_LEN) {
        errno = ERANGE;
        snprintf(result.error_message, sizeof(result.error_message),
                "CSV line too long: %zu characters", line_len);
        return result;
    }

    /* Count fields first */
    size_t field_count = 1;  /* At least one field even if empty */
    int in_quotes = 0;

    for (size_t i = 0; i < line_len; i++) {
        if (line[i] == '"') {
            in_quotes = !in_quotes;
        } else if (line[i] == delimiter && !in_quotes) {
            field_count++;
        }
    }

    /* Check for unclosed quotes */
    if (in_quotes) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "Unclosed quote in CSV line");
        return result;
    }

    /* Validate field count is reasonable */
    const size_t MAX_CSV_FIELDS = 1000;
    if (field_count > MAX_CSV_FIELDS) {
        errno = ERANGE;
        snprintf(result.error_message, sizeof(result.error_message),
                "Too many CSV fields: %zu", field_count);
        return result;
    }

    /* Allocate result buffer (conservative estimate) */
    size_t result_size = line_len + field_count * 2 + 64;  /* Extra space for formatting */
    char *parsed_result = malloc(result_size);
    if (!parsed_result) {
        errno = ENOMEM;
        snprintf(result.error_message, sizeof(result.error_message),
                "Cannot allocate %zu bytes", result_size);
        return result;
    }

    /* Parse fields */
    char *output_pos = parsed_result;
    size_t remaining_space = result_size;
    const char *field_start = line;
    in_quotes = 0;

    snprintf(output_pos, remaining_space, "CSV Fields (%zu):\n", field_count);
    size_t header_len = strlen(output_pos);
    output_pos += header_len;
    remaining_space -= header_len;

    size_t current_field = 0;
    for (size_t i = 0; i <= line_len; i++) {  /* Include null terminator in loop */
        char current_char = (i < line_len) ? line[i] : '\0';

        if (current_char == '"') {
            in_quotes = !in_quotes;
        } else if ((current_char == delimiter || current_char == '\0') && !in_quotes) {
            /* Extract field */
            size_t field_len = (size_t)(line + i - field_start);

            /* Remove surrounding quotes if present */
            const char *field_content = field_start;
            size_t content_len = field_len;

            if (field_len >= 2 && field_start[0] == '"' && field_start[field_len - 1] == '"') {
                field_content = field_start + 1;
                content_len = field_len - 2;
            }

            /* Add field to result */
            int written = snprintf(output_pos, remaining_space, "  [%zu]: \"", current_field);
            if (written < 0 || (size_t)written >= remaining_space) {
                free(parsed_result);
                errno = ENOSPC;
                snprintf(result.error_message, sizeof(result.error_message),
                        "Output buffer too small");
                return result;
            }
            output_pos += written;
            remaining_space -= (size_t)written;

            /* Copy field content, escaping any internal quotes */
            for (size_t j = 0; j < content_len && remaining_space > 3; j++) {
                if (field_content[j] == '"') {
                    if (remaining_space < 3) break;
                    *output_pos++ = '\\';
                    *output_pos++ = '"';
                    remaining_space -= 2;
                } else {
                    *output_pos++ = field_content[j];
                    remaining_space--;
                }
            }

            written = snprintf(output_pos, remaining_space, "\"\n");
            if (written < 0 || (size_t)written >= remaining_space) {
                free(parsed_result);
                errno = ENOSPC;
                snprintf(result.error_message, sizeof(result.error_message),
                        "Output buffer too small");
                return result;
            }
            output_pos += written;
            remaining_space -= (size_t)written;

            /* Move to next field */
            field_start = line + i + 1;
            current_field++;
        }
    }

    result.success = 1;
    result.result = parsed_result;
    result.result_length = result_size - remaining_space;
    snprintf(result.error_message, sizeof(result.error_message),
            "Parsed %zu CSV fields successfully", field_count);

    return result;
}

/* COMPLIANT: Safe configuration file parsing */
ParseResult safe_parse_config_entries(const char *config_text) {
    ParseResult result = {0, NULL, 0, ""};

    /* Validate parameter */
    if (!config_text) {
        errno = EINVAL;
        snprintf(result.error_message, sizeof(result.error_message),
                "NULL config text");
        return result;
    }

    size_t text_len = strlen(config_text);

    /* Check for reasonable size */
    const size_t MAX_CONFIG_SIZE = 1024 * 1024;  /* 1 MB */
    if (text_len > MAX_CONFIG_SIZE) {
        errno = ERANGE;
        snprintf(result.error_message, sizeof(result.error_message),
                "Config text too large: %zu bytes", text_len);
        return result;
    }

    /* Create working copy */
    char *text_copy = malloc(text_len + 1);
    if (!text_copy) {
        errno = ENOMEM;
        snprintf(result.error_message, sizeof(result.error_message),
                "Cannot allocate working buffer");
        return result;
    }
    strcpy(text_copy, config_text);

    /* Count valid configuration entries */
    size_t entry_count = 0;
    char *line = strtok(text_copy, "\n\r");

    while (line && entry_count < 10000) {  /* Limit entries */
        /* Skip empty lines and comments */
        while (*line && isspace(*line)) line++;  /* Trim leading whitespace */

        if (*line && *line != '#' && *line != ';') {
            /* Look for key=value format */
            char *equals = strchr(line, '=');
            if (equals && equals != line) {  /* Must have content before = */
                entry_count++;
            }
        }

        line = strtok(NULL, "\n\r");
    }

    free(text_copy);  /* Free working copy */

    if (entry_count == 0) {
        result.success = 1;
        result.result = malloc(1);
        if (result.result) {
            result.result[0] = '\0';
            result.result_length = 0;
        }
        snprintf(result.error_message, sizeof(result.error_message),
                "No configuration entries found");
        return result;
    }

    /* Allocate result buffer */
    size_t result_size = text_len * 2 + entry_count * 64;  /* Conservative estimate */
    char *parsed_config = malloc(result_size);
    if (!parsed_config) {
        errno = ENOMEM;
        snprintf(result.error_message, sizeof(result.error_message),
                "Cannot allocate result buffer");
        return result;
    }

    /* Parse configuration entries */
    text_copy = malloc(text_len + 1);
    if (!text_copy) {
        free(parsed_config);
        errno = ENOMEM;
        snprintf(result.error_message, sizeof(result.error_message),
                "Cannot allocate working buffer");
        return result;
    }
    strcpy(text_copy, config_text);

    char *output_pos = parsed_config;
    size_t remaining_space = result_size;

    int written = snprintf(output_pos, remaining_space, "Configuration Entries (%zu):\n", entry_count);
    output_pos += written;
    remaining_space -= (size_t)written;

    line = strtok(text_copy, "\n\r");
    size_t processed_entries = 0;

    while (line && processed_entries < entry_count && remaining_space > 100) {
        /* Trim leading whitespace */
        while (*line && isspace(*line)) line++;

        if (*line && *line != '#' && *line != ';') {
            char *equals = strchr(line, '=');
            if (equals && equals != line) {
                /* Extract key and value */
                *equals = '\0';
                char *key = line;
                char *value = equals + 1;

                /* Trim whitespace from key */
                char *key_end = equals - 1;
                while (key_end > key && isspace(*key_end)) {
                    *key_end = '\0';
                    key_end--;
                }

                /* Trim whitespace from value */
                while (*value && isspace(*value)) value++;
                char *value_end = value + strlen(value) - 1;
                while (value_end > value && isspace(*value_end)) {
                    *value_end = '\0';
                    value_end--;
                }

                /* Add to result */
                written = snprintf(output_pos, remaining_space, "  %s = \"%s\"\n", key, value);
                if (written > 0 && (size_t)written < remaining_space) {
                    output_pos += written;
                    remaining_space -= (size_t)written;
                    processed_entries++;
                }
            }
        }

        line = strtok(NULL, "\n\r");
    }

    free(text_copy);

    result.success = 1;
    result.result = parsed_config;
    result.result_length = result_size - remaining_space;
    snprintf(result.error_message, sizeof(result.error_message),
            "Parsed %zu configuration entries", processed_entries);

    return result;
}

int main(void) {
    printf("=== Defensive Parsing Functions Demo ===\n\n");

    /* Test integer parsing */
    printf("1. Integer parsing tests:\n");
    long int_result;
    if (safe_parse_integer("12345", &int_result, 10) == 0) {
        printf("   Parsed integer: %ld\n", int_result);
    }

    if (safe_parse_integer("0xFF", &int_result, 16) == 0) {
        printf("   Parsed hex: %ld\n", int_result);
    }

    if (safe_parse_integer("invalid123", &int_result, 10) != 0) {
        printf("   Correctly rejected invalid integer: %s\n", strerror(errno));
    }

    if (safe_parse_integer(NULL, &int_result, 10) != 0) {
        printf("   Correctly rejected NULL string: %s\n", strerror(errno));
    }

    /* Test floating-point parsing */
    printf("\n2. Floating-point parsing tests:\n");
    double double_result;
    if (safe_parse_double("123.456", &double_result) == 0) {
        printf("   Parsed double: %.3f\n", double_result);
    }

    if (safe_parse_double("1.23e-4", &double_result) == 0) {
        printf("   Parsed scientific notation: %.6f\n", double_result);
    }

    if (safe_parse_double("not.a.number", &double_result) != 0) {
        printf("   Correctly rejected invalid double: %s\n", strerror(errno));
    }

    /* Test CSV parsing */
    printf("\n3. CSV parsing tests:\n");
    const char *csv_line = "\"John Doe\",30,\"Software Engineer\",\"New York\"";
    ParseResult csv_result = safe_parse_csv_line(csv_line, ',');
    if (csv_result.success) {
        printf("   %s\n", csv_result.error_message);
        printf("   Result:\n%s", csv_result.result);
        free(csv_result.result);
    } else {
        printf("   Error: %s\n", csv_result.error_message);
    }

    /* Test CSV with invalid data */
    ParseResult csv_bad = safe_parse_csv_line("field1,\"unclosed quote,field3", ',');
    if (!csv_bad.success) {
        printf("   Correctly rejected malformed CSV: %s\n", csv_bad.error_message);
    }

    /* Test configuration parsing */
    printf("\n4. Configuration parsing tests:\n");
    const char *config_text =
        "# Configuration file\n"
        "server_host = localhost\n"
        "server_port = 8080\n"
        "debug_mode = true\n"
        "\n"
        "; Comment with semicolon\n"
        "max_connections = 100\n";

    ParseResult config_result = safe_parse_config_entries(config_text);
    if (config_result.success) {
        printf("   %s\n", config_result.error_message);
        printf("   Result:\n%s", config_result.result);
        free(config_result.result);
    } else {
        printf("   Error: %s\n", config_result.error_message);
    }

    /* Test configuration with NULL input */
    ParseResult config_null = safe_parse_config_entries(NULL);
    if (!config_null.success) {
        printf("   Correctly rejected NULL config: %s\n", config_null.error_message);
    }

    printf("\n=== Parsing functions demo completed ===\n");
    return 0;
}