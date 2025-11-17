/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include <unistd.h>

regex_t regex;

void regex_handler(int sig) {
    char pattern[] = "signal[0-9]+";
    char text[100];

    sprintf(text, "signal%d received", sig);  // sprintf also unsafe

    // VIOLATION: regcomp() is not async-safe
    int ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret != 0) {
        return;
    }

    // VIOLATION: regexec() is not async-safe
    regmatch_t matches[1];
    ret = regexec(&regex, text, 1, matches, 0);

    if (ret == 0) {
        // VIOLATION: Complex string processing based on regex results
        int start = matches[0].rm_so;
        int end = matches[0].rm_eo;
        char *match = malloc(end - start + 1);
        if (match) {
            strncpy(match, text + start, end - start);
            match[end - start] = '\0';
            free(match);
        }
    }

    // VIOLATION: regfree() is not async-safe
    regfree(&regex);

    // VIOLATION: regerror() is not async-safe
    if (ret != 0) {
        char error_buffer[256];
        regerror(ret, &regex, error_buffer, sizeof(error_buffer));
    }
}

int main() {
    printf("Demonstrating unsafe regex functions in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, regex_handler);

    printf("Send SIGUSR1 to trigger unsafe regex operations\n");

    while (1) {
        pause();
    }

    return 0;
}