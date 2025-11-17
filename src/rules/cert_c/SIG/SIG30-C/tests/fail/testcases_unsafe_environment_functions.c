/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void env_handler(int sig) {
    // VIOLATION: getenv() is not async-safe
    char *path = getenv("PATH");
    char *home = getenv("HOME");
    char *user = getenv("USER");

    // VIOLATION: putenv() is not async-safe
    putenv("SIGNAL_RECEIVED=1");

    // VIOLATION: setenv() is not async-safe
    setenv("SIGNAL_TYPE", "SIGUSR1", 1);

    // VIOLATION: unsetenv() is not async-safe
    unsetenv("TEMP_VAR");

    // VIOLATION: clearenv() is not async-safe (if available)
#ifdef __USE_MISC
    // clearenv();  // Would clear all environment variables
#endif

    // VIOLATION: Using environment-dependent operations
    if (path != NULL) {
        // Processing PATH variable - involves string operations
        char *token = strtok(path, ":");  // strtok is not async-safe
    }

    // VIOLATION: chdir() affects environment and is not async-safe
    if (home != NULL) {
        chdir(home);
    }
}

int main() {
    printf("Demonstrating unsafe environment functions in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, env_handler);

    printf("Send SIGUSR1 to trigger unsafe environment operations\n");

    while (1) {
        pause();
    }

    return 0;
}