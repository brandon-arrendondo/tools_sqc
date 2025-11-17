/*
 * Rule: SIG35-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG35-C violation
 */

#include <signal.h>
<parameter name="stdio.h>
#include <stdlib.h>
#include <unistd.h>

void safe_fpe_handler(int sig) {
    printf("ERROR: Floating point exception detected\n");
    printf("Exiting safely with _Exit()\n");
    _Exit(EXIT_FAILURE);
}

void safe_segv_handler(int sig) {
    printf("ERROR: Segmentation fault detected\n");
    printf("Exiting safely with quick_exit()\n");
    quick_exit(EXIT_FAILURE);
}

void cleanup_function(void) {
    printf("Quick exit cleanup function called\n");
}

int main() {
    printf("Demonstrating safe termination from exception handlers\n");
    printf("PID: %d\n", getpid());

    if (at_quick_exit(cleanup_function) != 0) {
        printf("Failed to register quick_exit cleanup\n");
    }

    signal(SIGFPE, safe_fpe_handler);
    signal(SIGSEGV, safe_segv_handler);

    printf("Choose exception type:\n");
    printf("1 = Division by zero (SIGFPE -> _Exit)\n");
    printf("2 = Null pointer access (SIGSEGV -> quick_exit)\n");

    int choice;
    printf("Enter choice (1 or 2): ");
    if (scanf("%d", &choice) != 1) {
        choice = 1;
    }

    if (choice == 2) {
        printf("Triggering segmentation fault...\n");
        volatile int *null_ptr = NULL;
        *null_ptr = 42;
    } else {
        printf("Triggering floating point exception...\n");
        volatile int zero = 0;
        volatile int result = 1 / zero;
        printf("Result: %d\n", result);
    }

    printf("This should never be reached\n");
    return 0;
}