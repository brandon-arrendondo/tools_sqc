/*
 * Rule: STR00-C
 * Source: testcases
 * Status: FAIL - Should trigger STR00-C violation
 */

/*
 * CERT C STR00-C Fail Case: string_concatenation_types.c
 *
 * This case demonstrates a violation of STR00-C by using inappropriate
 * character types for string concatenation operations, leading to
 * type warnings and potential data handling issues.
 */

#include <stdio.h>
#include <string.h>

int main(void) {
    /* VIOLATION: String concatenation with mixed character types */
    char dest_plain[200] = "Plain: ";
    signed char src_signed[] = "Signed source";
    unsigned char src_unsigned[] = "Unsigned source";

    printf("String concatenation with type mismatches:\n");

    /* VIOLATION: strcat with incompatible types */
    strcat(dest_plain, src_signed);     /* Warning: incompatible pointer types */
    printf("After signed concat: %s\n", dest_plain);

    strcat(dest_plain, " + ");          /* OK */
    strcat(dest_plain, src_unsigned);   /* Warning: incompatible pointer types */
    printf("After unsigned concat: %s\n", dest_plain);

    /* VIOLATION: strncat with type mismatches */
    signed char dest_signed[100] = "Signed: ";
    strncat(dest_signed, dest_plain, 20);     /* Warning */
    printf("Signed destination: %s\n", dest_signed);

    /* VIOLATION: Manual concatenation with wrong types */
    unsigned char manual_dest[150];
    unsigned char prefix[] = "Manual: ";

    /* Copy prefix */
    strcpy(manual_dest, prefix);              /* OK - same types */

    /* VIOLATION: Append with different character types */
    size_t current_len = strlen((char*)manual_dest);
    char *append_pos = (char*)(manual_dest + current_len);

    strcpy(append_pos, src_signed);           /* Warning: type mismatch */

    printf("Manual concatenation: %s\n", manual_dest);

    /* VIOLATION: Character-by-character concatenation */
    char result_buffer[300] = "Result: ";
    size_t result_len = strlen(result_buffer);

    /* Append signed char array character by character */
    for (size_t i = 0; src_signed[i] != '\0'; i++) {
        result_buffer[result_len + i] = src_signed[i];  /* Warning */
    }
    result_len += strlen((char*)src_signed);
    result_buffer[result_len] = '\0';

    /* Append separator */
    strcat(result_buffer, " | ");
    result_len = strlen(result_buffer);

    /* Append unsigned char array */
    for (size_t i = 0; src_unsigned[i] != '\0'; i++) {
        result_buffer[result_len + i] = src_unsigned[i];  /* Warning */
    }
    result_len += strlen((char*)src_unsigned);
    result_buffer[result_len] = '\0';

    printf("Character-by-character result: %s\n", result_buffer);

    /* VIOLATION: sprintf with mixed character types */
    char sprintf_buffer[200];
    sprintf(sprintf_buffer, "Formatted: %s and %s",
            src_signed, src_unsigned);  /* Warnings for both %s */

    printf("sprintf result: %s\n", sprintf_buffer);

    /* VIOLATION: Dynamic string building with wrong types */
    signed char dynamic_parts[][20] = {
        "Part1", "Part2", "Part3"
    };

    char dynamic_result[100] = "";
    for (int i = 0; i < 3; i++) {
        if (i > 0) {
            strcat(dynamic_result, "-");
        }
        strcat(dynamic_result, dynamic_parts[i]);  /* Warning */
    }

    printf("Dynamic result: %s\n", dynamic_result);

    /* VIOLATION: String building with pointer manipulation */
    unsigned char *build_ptr = (unsigned char*)malloc(200);
    if (build_ptr != NULL) {
        strcpy(build_ptr, "Built: ");           /* Warning */

        /* Find end of current string */
        unsigned char *end_ptr = build_ptr + strlen((char*)build_ptr);

        /* Append more strings */
        strcpy(end_ptr, src_signed);            /* Warning */
        end_ptr += strlen((char*)src_signed);

        strcpy(end_ptr, " & ");                 /* Warning */
        end_ptr += 3;

        strcpy(end_ptr, src_unsigned);          /* Warning */

        printf("Built string: %s\n", build_ptr);
        free(build_ptr);
    }

    /* VIOLATION: Concatenation in loops with type issues */
    char loop_result[300] = "Loop: ";
    signed char items[][10] = {"item1", "item2", "item3"};

    for (int i = 0; i < 3; i++) {
        if (i > 0) {
            strcat(loop_result, ", ");
        }
        strcat(loop_result, items[i]);          /* Warning */
    }

    printf("Loop concatenation: %s\n", loop_result);

    /* VIOLATION: Wide character concatenation with narrow types */
    wchar_t wide_prefix[] = L"Wide: ";
    char narrow_suffix[] = "narrow";

    /* This is fundamentally wrong - mixing wide and narrow */
    wchar_t wide_result[100];
    wcscpy(wide_result, wide_prefix);

    /* VIOLATION: Trying to append narrow to wide */
    wcscat(wide_result, (wchar_t*)narrow_suffix);  /* Wrong cast and data corruption */

    printf("Wide concatenation result (corrupted): %ls\n", wide_result);

    return 0;
}