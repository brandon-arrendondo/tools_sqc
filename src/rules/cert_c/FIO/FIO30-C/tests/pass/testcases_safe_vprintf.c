/*
 * Rule: FIO30-C
 * Source: testcases
 * Status: PASS - Should NOT trigger FIO30-C violation
 */

/*
 * Rule: FIO30-C - Exclude user input from format strings
 * Status: PASS
 * Reason: Uses vprintf with literal format string and proper variadic handling
 */

#include <stdio.h>
#include <stdarg.h>

void safe_printf_wrapper(const char *format, ...) {
    va_list args;
    va_start(args, format);

    // Safe: format string is parameter from controlled source
    vprintf(format, args);

    va_end(args);
}

int main() {
    char user_name[50];
    int user_score;

    printf("Enter player name: ");
    scanf("%49s", user_name);
    printf("Enter score: ");
    scanf("%d", &user_score);

    // Safe: literal format string passed to wrapper
    safe_printf_wrapper("Player: %s scored %d points\n", user_name, user_score);

    return 0;
}