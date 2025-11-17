/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: token_parsing_wrong_types.c
 *
 * This case demonstrates a violation of STR00-C by using inappropriate
 * character types for token parsing and string manipulation operations,
 * leading to type warnings and inconsistent behavior.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
    /* VIOLATION: Using signed char for token parsing */
    signed char input_signed[] = "apple,banana,cherry,date";
    signed char delimiters[] = ",";

    printf("Token parsing with signed char:\n");

    /* VIOLATION: strtok with signed char */
    signed char *token_signed = strtok(input_signed, delimiters);  /* Warning */
    int count = 0;
    while (token_signed != NULL) {
        printf("Token %d: %s\n", ++count, token_signed);  /* Warning */
        token_signed = strtok(NULL, delimiters);           /* Warning */
    }

    /* VIOLATION: Using unsigned char for token parsing */
    unsigned char input_unsigned[] = "red;green;blue;yellow";
    unsigned char sep[] = ";";

    printf("\nToken parsing with unsigned char:\n");

    /* Create a copy for strtok (since it modifies the string) */
    unsigned char *input_copy = malloc(strlen((char*)input_unsigned) + 1);
    if (input_copy == NULL) return 1;

    strcpy(input_copy, input_unsigned);  /* Warning */

    /* VIOLATION: strtok with unsigned char */
    unsigned char *token_unsigned = strtok(input_copy, sep);  /* Warning */
    count = 0;
    while (token_unsigned != NULL) {
        printf("Token %d: %s\n", ++count, token_unsigned);  /* Warning */
        token_unsigned = strtok(NULL, sep);                  /* Warning */
    }

    free(input_copy);

    /* VIOLATION: Manual token parsing with wrong character types */
    signed char manual_input[] = "word1:word2:word3:word4";
    signed char separator = ':';

    printf("\nManual parsing with signed char:\n");

    signed char *start = manual_input;
    signed char *end;
    count = 0;

    while ((end = strchr(start, separator)) != NULL) {  /* Warning */
        /* VIOLATION: Null termination with character type issues */
        *end = '\0';
        printf("Manual token %d: %s\n", ++count, start);  /* Warning */
        start = end + 1;
    }

    /* Print the last token */
    if (*start != '\0') {
        printf("Manual token %d: %s\n", ++count, start);  /* Warning */
    }

    /* VIOLATION: String splitting with character type mismatches */
    char plain_data[] = "name=John,age=25,city=NYC";
    unsigned char key_value_sep = '=';
    signed char pair_sep[] = ",";

    printf("\nKey-value parsing with mixed types:\n");

    /* Create working copy */
    char *data_copy = malloc(strlen(plain_data) + 1);
    if (data_copy == NULL) return 1;
    strcpy(data_copy, plain_data);

    /* VIOLATION: strtok with mixed character types */
    char *pair = strtok(data_copy, pair_sep);  /* Warning */
    while (pair != NULL) {
        /* VIOLATION: strchr with type mismatch */
        char *equals = strchr(pair, key_value_sep);  /* Warning */
        if (equals != NULL) {
            *equals = '\0';
            printf("Key: %s, Value: %s\n", pair, equals + 1);
        }
        pair = strtok(NULL, pair_sep);  /* Warning */
    }

    free(data_copy);

    /* VIOLATION: CSV parsing with wrong character types */
    signed char csv_line[] = "\"Smith, John\",30,\"New York, NY\",Engineer";

    printf("\nCSV parsing with signed char:\n");

    signed char *field_start = csv_line;
    signed char *current = csv_line;
    int field_num = 1;
    int in_quotes = 0;

    while (*current != '\0') {
        if (*current == '"') {
            in_quotes = !in_quotes;
        } else if (*current == ',' && !in_quotes) {
            *current = '\0';
            printf("Field %d: %s\n", field_num++, field_start);  /* Warning */
            field_start = current + 1;
        }
        current++;
    }

    /* Print last field */
    printf("Field %d: %s\n", field_num, field_start);  /* Warning */

    /* VIOLATION: Path parsing with character type issues */
    unsigned char file_path[] = "/home/user/documents/file.txt";
    unsigned char path_sep = '/';

    printf("\nPath parsing with unsigned char:\n");

    unsigned char *path_copy = malloc(strlen((char*)file_path) + 1);
    if (path_copy == NULL) return 1;
    strcpy(path_copy, file_path);  /* Warning */

    /* Find filename (last component) */
    unsigned char *filename = strrchr(path_copy, path_sep);  /* Warning */
    if (filename != NULL) {
        filename++;  /* Skip the separator */
        printf("Filename: %s\n", filename);  /* Warning */

        /* Find extension */
        unsigned char *extension = strrchr(filename, '.');  /* Warning */
        if (extension != NULL) {
            printf("Extension: %s\n", extension);  /* Warning */
        }
    }

    free(path_copy);

    /* VIOLATION: Command line argument parsing simulation */
    signed char command_line[] = "--input file.txt --output result.txt --verbose";
    signed char *args[10];
    int arg_count = 0;

    printf("\nCommand line parsing with signed char:\n");

    /* Simple space-separated parsing */
    signed char *arg = strtok(command_line, " ");  /* Warning */
    while (arg != NULL && arg_count < 10) {
        args[arg_count++] = arg;
        arg = strtok(NULL, " ");  /* Warning */
    }

    for (int i = 0; i < arg_count; i++) {
        printf("Arg %d: %s\n", i, args[i]);  /* Warning */
    }

    /* VIOLATION: URL parsing with mixed character types */
    char url[] = "https://example.com:8080/path/to/resource?param1=value1&param2=value2";
    unsigned char protocol_sep[] = "://";
    signed char port_sep = ':';
    char path_sep_char = '/';
    unsigned char query_sep = '?';

    printf("\nURL parsing with mixed character types:\n");

    /* Find protocol */
    char *protocol_end = strstr(url, (char*)protocol_sep);  /* Warning */
    if (protocol_end != NULL) {
        *protocol_end = '\0';
        printf("Protocol: %s\n", url);

        char *host_start = protocol_end + strlen((char*)protocol_sep);
        char *port_start = strchr(host_start, port_sep);  /* Warning */

        if (port_start != NULL) {
            *port_start = '\0';
            printf("Host: %s\n", host_start);

            port_start++;
            char *path_start = strchr(port_start, path_sep_char);
            if (path_start != NULL) {
                *path_start = '\0';
                printf("Port: %s\n", port_start);
                printf("Path starts at: %c\n", *path_start);
            }
        }
    }

    return 0;
}