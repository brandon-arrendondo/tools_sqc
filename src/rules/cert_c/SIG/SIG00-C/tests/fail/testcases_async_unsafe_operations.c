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

char global_buffer[1000];
int buffer_pos = 0;

void unsafe_signal_handler(int sig) {
    char msg[100];
    sprintf(msg, "Signal %d at position %d\n", sig, buffer_pos);

    strcat(global_buffer, msg);
    buffer_pos += strlen(msg);

    printf("Buffer updated: %s", global_buffer);

    malloc(100);
    free(malloc(50));
}

int main() {
    signal(SIGUSR1, unsafe_signal_handler);

    printf("PID: %d\n", getpid());

    for (int i = 0; i < 1000; i++) {
        sprintf(global_buffer + buffer_pos, "Main: %d ", i);
        buffer_pos += 8;

        if (i % 100 == 0) {
            printf("Main progress: %d\n", i);
        }

        usleep(1000);
    }

    return 0;
}