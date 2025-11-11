/*
 * Rule: SIG00-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG00-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

volatile sig_atomic_t timestamp_count = 0;

void library_handler(int sig) {
    timestamp_count++;

    printf("Handler: Signal %d received\n", sig);

    // Violation: Calling non-async-safe library functions
    // without proper signal masking
    time_t current_time = time(NULL);
    char* time_str = ctime(&current_time);

    if (time_str != NULL) {
        // Remove newline
        time_str[strlen(time_str) - 1] = '\0';
        printf("Handler: Time is %s\n", time_str);
    }

    // More non-async-safe calls
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "Signal %d at timestamp %ld",
             sig, (long)current_time);

    printf("Handler: %s\n", buffer);

    // getenv is not async-safe
    char* path = getenv("PATH");
    if (path) {
        printf("Handler: PATH length is %zu\n", strlen(path));
    }

    sleep(1); // Increase interruption window
}

int main() {
    struct sigaction sa;

    // Install handler without masking
    sa.sa_handler = library_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: Library functions can be corrupted by interruption
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Send signals to trigger non-async-safe library calls\n");

    while (1) {
        // Main thread also uses library functions
        time_t main_time = time(NULL);
        printf("Main: Current time is %ld, signals: %d\n",
               (long)main_time, timestamp_count);

        char* user = getenv("USER");
        if (user) {
            printf("Main: User is %s\n", user);
        }

        sleep(3);
    }

    return 0;
}