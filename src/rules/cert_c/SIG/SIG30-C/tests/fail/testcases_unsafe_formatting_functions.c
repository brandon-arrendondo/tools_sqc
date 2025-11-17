/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

void custom_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);  // VIOLATION: vprintf is not async-safe
    va_end(args);
}

void formatting_handler(int sig) {
    char buffer[256];

    // VIOLATION: sprintf family functions are not async-safe
    sprintf(buffer, "Signal %d received", sig);
    snprintf(buffer, sizeof(buffer), "Safe signal %d", sig);

    // VIOLATION: printf family functions are not async-safe
    printf("Signal number: %d\n", sig);
    fprintf(stdout, "Signal to stdout: %d\n", sig);
    fprintf(stderr, "Signal to stderr: %d\n", sig);

    // VIOLATION: scanf family functions are not async-safe
    // Note: These would block, but demonstrate the violation
    int dummy;
    sscanf("123", "%d", &dummy);

    // VIOLATION: Variable argument functions
    va_list args;
    // Using our custom printf function
    custom_printf("Custom format: signal %d\n", sig);

    // VIOLATION: vsprintf, vfprintf are not async-safe
    va_start(args, sig);  // This won't work properly, but shows the concept
    vsprintf(buffer, "Variable args: %d", args);
    va_end(args);

    // VIOLATION: dprintf may not be async-safe on all systems
    dprintf(STDOUT_FILENO, "Direct printf: %d\n", sig);

    // VIOLATION: asprintf (GNU extension) is not async-safe
#ifdef __GNU_LIBRARY__
    char *allocated_str;
    asprintf(&allocated_str, "Allocated string: %d", sig);
    if (allocated_str) {
        printf("%s\n", allocated_str);
        free(allocated_str);  // Also unsafe
    }
#endif

    // VIOLATION: Using format strings with complex specifiers
    printf("Complex format: %+08.2f %#x %*s\n", 3.14159, sig, 10, "test");
}

int main() {
    printf("Demonstrating unsafe formatting functions in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, formatting_handler);

    printf("Send SIGUSR1 to trigger unsafe formatting operations\n");

    while (1) {
        pause();
    }

    return 0;
}