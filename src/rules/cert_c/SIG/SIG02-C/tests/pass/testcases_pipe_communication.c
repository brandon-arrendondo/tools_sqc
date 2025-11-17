/*
 * Rule: SIG02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG02-C violation
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    int pipefd[2];
    pid_t child_pid;
    char buffer[256];

    printf("Using pipes for normal inter-process communication (GOOD)\n");

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    child_pid = fork();
    if (child_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
        close(pipefd[0]);

        sleep(1);
        printf("Child: Sending data ready message\n");
        write(pipefd[1], "DATA_READY", 11);

        sleep(2);
        printf("Child: Sending process complete message\n");
        write(pipefd[1], "COMPLETE", 9);

        close(pipefd[1]);
        exit(0);
    } else {
        close(pipefd[1]);

        printf("Parent waiting for data ready message...\n");
        if (read(pipefd[0], buffer, sizeof(buffer)) > 0) {
            printf("Parent: Received: %s\n", buffer);
        }

        printf("Parent: Processing data...\n");
        sleep(1);

        printf("Parent waiting for completion message...\n");
        if (read(pipefd[0], buffer, sizeof(buffer)) > 0) {
            printf("Parent: Received: %s\n", buffer);
        }

        printf("Parent: All done\n");
        close(pipefd[0]);
        wait(NULL);
    }

    return 0;
}