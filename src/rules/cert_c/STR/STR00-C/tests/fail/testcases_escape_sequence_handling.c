/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: escape_sequence_handling.c
 *
 * This case demonstrates a violation of STR00-C by inappropriately
 * handling escape sequences with different character types, leading to
 * sign-dependent interpretation and potential data corruption.
 */

#include <stdio.h>
#include <string.h>

int main(void) {
    /* VIOLATION: Escape sequences with signed char */
    signed char signed_escapes[] = "Line1\nLine2\tTabbed\rCarriage\aAlert\bBackspace";

    printf("Escape sequences with signed char:\n");
    printf("String: %s\n", signed_escapes);  /* Warning */

    /* VIOLATION: Examining escape sequence values */
    printf("Escape sequence values in signed char:\n");
    for (size_t i = 0; signed_escapes[i] != '\0'; i++) {
        signed char c = signed_escapes[i];
        if (c < 32) {  /* Control characters */
            printf("Position %zu: control character (value: %d)\n", i, c);
        }
    }

    /* VIOLATION: High-value escape sequences */
    signed char high_escapes[] = "\x80\x90\xA0\xFF";  /* May be negative */
    unsigned char uhigh_escapes[] = "\x80\x90\xA0\xFF";

    printf("\nHigh-value escape sequences:\n");
    printf("Signed char values:\n");
    for (size_t i = 0; i < 4; i++) {
        printf("  \\x%02X as signed: %d\n",
               (unsigned char)high_escapes[i], high_escapes[i]);
    }

    printf("Unsigned char values:\n");
    for (size_t i = 0; i < 4; i++) {
        printf("  \\x%02X as unsigned: %d\n",
               uhigh_escapes[i], uhigh_escapes[i]);
    }

    /* VIOLATION: Octal escape sequences with sign issues */
    signed char octal_escapes[] = "\101\102\103\377";  /* A, B, C, 0xFF */

    printf("\nOctal escape sequences:\n");
    for (size_t i = 0; octal_escapes[i] != '\0'; i++) {
        signed char c = octal_escapes[i];
        printf("Octal char %zu: %d (as char: '%c')\n", i, c,
               (c >= 32 && c < 127) ? c : '?');
    }

    /* VIOLATION: String processing with escape sequences */
    char mixed_content[] = "Normal text\n\tWith escapes\x80\x90";
    signed char *signed_ptr = (signed char*)mixed_content;

    printf("\nString processing with escapes:\n");
    while (*signed_ptr != '\0') {
        if (*signed_ptr == '\n') {
            printf("\\n");
        } else if (*signed_ptr == '\t') {
            printf("\\t");
        } else if (*signed_ptr < 0) {  /* Sign-dependent check */
            printf("\\x%02X", (unsigned char)*signed_ptr);
        } else if (*signed_ptr >= 32 && *signed_ptr < 127) {
            printf("%c", *signed_ptr);
        } else {
            printf("\\x%02X", (unsigned char)*signed_ptr);
        }
        signed_ptr++;
    }
    printf("\n");

    /* VIOLATION: Escape sequence manipulation */
    unsigned char buffer[100];
    const char source[] = "Text with\nnewlines\tand\ttabs";

    printf("\nEscape sequence manipulation:\n");

    /* Copy and modify escape sequences */
    size_t j = 0;
    for (size_t i = 0; source[i] != '\0' && j < 99; i++) {
        char c = source[i];

        if (c == '\n') {
            /* Replace newline with space */
            buffer[j++] = ' ';
        } else if (c == '\t') {
            /* Replace tab with multiple spaces */
            buffer[j++] = ' ';
            buffer[j++] = ' ';
            buffer[j++] = ' ';
            buffer[j++] = ' ';
        } else {
            /* VIOLATION: Assignment between different character types */
            buffer[j++] = c;  /* char to unsigned char */
        }
    }
    buffer[j] = '\0';

    printf("Modified string: %s\n", buffer);  /* Warning */

    /* VIOLATION: Path separator handling with character types */
    signed char windows_path[] = "C:\\Users\\Name\\Documents\\file.txt";
    signed char unix_path[] = "/home/user/documents/file.txt";

    printf("\nPath separator handling:\n");

    /* Count backslashes in Windows path */
    int backslash_count = 0;
    for (size_t i = 0; windows_path[i] != '\0'; i++) {
        if (windows_path[i] == '\\') {
            backslash_count++;
        }
    }
    printf("Backslashes in Windows path: %d\n", backslash_count);

    /* Convert backslashes to forward slashes */
    for (size_t i = 0; windows_path[i] != '\0'; i++) {
        if (windows_path[i] == '\\') {
            windows_path[i] = '/';
        }
    }
    printf("Converted path: %s\n", windows_path);  /* Warning */

    /* VIOLATION: JSON-style escape sequence handling */
    char json_string[] = "JSON: \"Hello\\nWorld\\t\\\"Quoted\\\"\"";
    unsigned char *ujson_ptr = (unsigned char*)json_string;

    printf("\nJSON escape processing:\n");
    printf("Processing: %s\n", ujson_ptr);  /* Warning */

    /* Simple escape sequence detection */
    for (size_t i = 0; ujson_ptr[i] != '\0'; i++) {
        if (ujson_ptr[i] == '\\' && ujson_ptr[i+1] != '\0') {
            printf("Escape sequence at position %zu: \\%c\n",
                   i, ujson_ptr[i+1]);
            i++;  /* Skip the escaped character */
        }
    }

    /* VIOLATION: Binary data with embedded escape sequences */
    signed char binary_data[] = {
        'H', 'e', 'l', 'l', 'o', '\0',  /* Null in middle */
        'W', 'o', 'r', 'l', 'd', '\n',
        '\x80', '\x90', '\xFF', '\0'     /* High-bit values and null terminator */
    };

    printf("\nBinary data with escapes:\n");
    printf("Data length (until first null): %zu\n", strlen((char*)binary_data));

    /* Process all bytes including those after first null */
    for (size_t i = 0; i < sizeof(binary_data) - 1; i++) {
        signed char c = binary_data[i];
        if (c == '\0') {
            printf("\\0 ");
        } else if (c == '\n') {
            printf("\\n ");
        } else if (c >= 32 && c < 127) {
            printf("%c ", c);
        } else {
            printf("\\x%02X ", (unsigned char)c);
        }
    }
    printf("\n");

    /* VIOLATION: URL encoding/decoding with character types */
    char url_encoded[] = "Hello%20World%21%40%23";
    signed char *decode_ptr = (signed char*)url_encoded;

    printf("\nURL encoding handling:\n");
    while (*decode_ptr != '\0') {
        if (*decode_ptr == '%' && decode_ptr[1] != '\0' && decode_ptr[2] != '\0') {
            /* Simple hex decode (simplified) */
            printf("Percent encoding: %%%c%c\n", decode_ptr[1], decode_ptr[2]);
            decode_ptr += 3;
        } else {
            printf("Regular char: %c\n", *decode_ptr);
            decode_ptr++;
        }
    }

    return 0;
}