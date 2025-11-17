/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: Wide character string conversion may exceed narrow character buffer
 */

#include <stdio.h>
#include <wchar.h>
#include <locale.h>

int main() {
    wchar_t wide_str[] = L"Unicode string with special characters: éñüñ";
    char narrow_buffer[20];  // Too small for multi-byte characters

    setlocale(LC_ALL, "");

    // Converting wide to narrow may need more bytes than characters
    wcstombs(narrow_buffer, wide_str, sizeof(narrow_buffer));
    printf("Converted: %s\n", narrow_buffer);

    return 0;
}