/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: wide_char_misuse.c
 *
 * This case demonstrates a violation of STR00-C by inappropriately
 * mixing wide character types with regular character operations
 * or using wrong types for wide character handling.
 */

#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>

int main(void) {
    setlocale(LC_ALL, "");

    /* VIOLATION: Using char for wide character storage */
    char narrow_buffer[100];
    wchar_t wide_string[] = L"Hello, 世界! 🌍";

    /* VIOLATION: Trying to store wide characters in narrow char array */
    /* This loses data and causes encoding issues */
    for (size_t i = 0; i < wcslen(wide_string) && i < 99; i++) {
        narrow_buffer[i] = (char)wide_string[i];  /* Data loss */
    }
    narrow_buffer[wcslen(wide_string)] = '\0';

    printf("Original wide string length: %zu\n", wcslen(wide_string));
    printf("Corrupted narrow string: %s\n", narrow_buffer);

    /* VIOLATION: Using narrow char functions on wide characters */
    wchar_t test_wide = L'A';
    char narrow_char = (char)test_wide;

    /* Wrong: using narrow char functions on wide char data */
    if (strlen((char*)wide_string) > 0) {  /* Wrong function, wrong cast */
        printf("Length calculation error\n");
    }

    /* VIOLATION: Mixing wide and narrow character constants */
    wchar_t mixed_string[50];
    mixed_string[0] = 'H';     /* Narrow char in wide context */
    mixed_string[1] = L'e';    /* Wide char - correct */
    mixed_string[2] = 'l';     /* Narrow char in wide context */
    mixed_string[3] = L'l';    /* Wide char - correct */
    mixed_string[4] = 'o';     /* Narrow char in wide context */
    mixed_string[5] = L'\0';   /* Wide null terminator */

    wprintf(L"Mixed string: %ls\n", mixed_string);

    /* VIOLATION: Using int for wide character when wchar_t is appropriate */
    int wide_as_int = L'ñ';    /* Should use wchar_t */
    printf("Wide char as int: %d\n", wide_as_int);

    /* VIOLATION: Wrong size assumptions for wide characters */
    wchar_t wide_array[10];
    size_t byte_size = sizeof(wide_array);
    size_t char_count = byte_size;  /* Wrong: assumes 1 byte per character */

    printf("Wide array byte size: %zu\n", byte_size);
    printf("Incorrect character count: %zu\n", char_count);
    printf("Correct character capacity: %zu\n", sizeof(wide_array) / sizeof(wchar_t));

    /* VIOLATION: Using narrow string functions on wide strings */
    wchar_t source[] = L"Source";
    wchar_t dest[20];

    /* Wrong function for wide strings */
    strcpy((char*)dest, (char*)source);  /* Dangerous cast and wrong function */

    /* VIOLATION: Character comparison with wrong types */
    wchar_t unicode_char = L'é';
    if (unicode_char == 'e') {  /* Comparing wide char with narrow char */
        printf("Characters match\n");  /* Unlikely to match */
    }

    /* VIOLATION: File I/O with wrong character types */
    FILE *file = fopen("test.txt", "w");
    if (file != NULL) {
        /* Wrong: writing wide characters with narrow I/O */
        for (size_t i = 0; wide_string[i] != L'\0'; i++) {
            fputc((char)wide_string[i], file);  /* Data loss */
        }
        fclose(file);
    }

    return 0;
}