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

volatile sig_atomic_t env_updates = 0;

void env_handler(int sig) {
    env_updates++;
    char var_name[64];
    char var_value[256];

    printf("Handler: Signal %d modifying environment\n", sig);

    // Violation: Environment variable operations without proper masking
    // putenv/setenv are not async-safe and can cause corruption
    snprintf(var_name, sizeof(var_name), "SIGNAL_%d_VAR", sig);
    snprintf(var_value, sizeof(var_value), "value_from_signal_%d_update_%d",
             sig, env_updates);

    printf("Handler: Setting %s=%s\n", var_name, var_value);

    // Non-async-safe environment modification
    if (setenv(var_name, var_value, 1) != 0) {
        perror("Handler: setenv failed");
        return;
    }

    // Additional environment operations
    char* path = getenv("PATH");
    if (path != NULL) {
        printf("Handler: PATH length = %zu\n", strlen(path));

        // Try to modify PATH (dangerous)
        char new_path[4096];
        snprintf(new_path, sizeof(new_path), "%s:/tmp/signal_%d", path, sig);

        // Create vulnerability window
        sleep(1);

        if (setenv("PATH", new_path, 1) != 0) {
            perror("Handler: PATH modification failed");
        } else {
            printf("Handler: PATH modified\n");
        }
    }

    // More non-async-safe operations
    system("echo 'Handler executed system command' >/tmp/handler_log");

    printf("Handler: Environment modification complete\n");
}

int main() {
    struct sigaction sa;

    // Install handler without masking
    sa.sa_handler = env_handler;
    sigemptyset(&sa.sa_mask);
    // Violation: Environment functions not async-safe
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("PID: %d\n", getpid());
    printf("Send signals to modify environment variables\n");

    while (1) {
        printf("Main: Environment updates: %d\n", env_updates);

        // Main thread also modifies environment
        char main_var[64];
        snprintf(main_var, sizeof(main_var), "MAIN_VAR_%d", (int)time(NULL));
        setenv(main_var, "main_thread_value", 1);

        // Check for signal-created variables
        char* signal_var = getenv("SIGNAL_10_VAR");
        if (signal_var) {
            printf("Main: Found SIGNAL_10_VAR = %s\n", signal_var);
        }

        // Print current PATH to detect corruption
        char* current_path = getenv("PATH");
        if (current_path) {
            if (strstr(current_path, "/tmp/signal_") != NULL) {
                printf("Main: PATH was modified by signal handler\n");
            }
        }

        sleep(3);
    }

    return 0;
}