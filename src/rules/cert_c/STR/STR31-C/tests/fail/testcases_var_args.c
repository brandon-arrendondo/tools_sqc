/*
 * Rule: STR31-C
 * Source: testcases
 * Status: FAIL - Should trigger STR31-C violation
 */

/*
 * Rule: STR31-C - Guarantee that storage for strings has sufficient space for character data and the null terminator
 * Status: FAIL
 * Reason: Variable arguments in sprintf can produce unpredictable output length
 */

#include <stdio.h>
#include <stdarg.h>

void format_message(char *buffer, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);  // No bounds checking on buffer
    va_end(args);
}

int main() {
    char small_buffer[10];

    format_message(small_buffer, "User %s has %d items in cart",
                  "VeryLongUsername", 999999);
    printf("Message: %s\n", small_buffer);

    return 0;
}