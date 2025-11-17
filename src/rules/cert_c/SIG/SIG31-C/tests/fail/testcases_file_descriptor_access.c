/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

typedef struct {
    int fd;
    char filename[256];
    int is_open;
    long bytes_written;
    long bytes_read;
} file_state_t;

file_state_t global_file_state = {-1, "", 0, 0, 0};
FILE *global_log_file = NULL;

void unsafe_handler(int sig) {
    /* Violation: Accessing file descriptors and I/O state in signal handler */
    char buffer[128];
    sprintf(buffer, "Signal %d received at handler\n", sig);

    if (global_file_state.is_open && global_file_state.fd >= 0) {
        int written = write(global_file_state.fd, buffer, strlen(buffer));
        global_file_state.bytes_written += written;
    }

    if (global_log_file) {
        fprintf(global_log_file, "Handler: Signal %d, bytes_written=%ld\n",
                sig, global_file_state.bytes_written);
        fflush(global_log_file);
    }

    printf("Handler: fd=%d, open=%d, written=%ld, read=%ld\n",
           global_file_state.fd, global_file_state.is_open,
           global_file_state.bytes_written, global_file_state.bytes_read);
}

int main() {
    printf("Demonstrating unsafe file descriptor access in signal handler\n");
    printf("PID: %d\n", getpid());

    /* Open test file */
    strcpy(global_file_state.filename, "/tmp/claude/test_signal_io.txt");
    global_file_state.fd = open(global_file_state.filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (global_file_state.fd >= 0) {
        global_file_state.is_open = 1;
    }

    global_log_file = fopen("/tmp/claude/signal_log.txt", "w");

    signal(SIGUSR1, unsafe_handler);

    for (int i = 0; i < 20; i++) {
        char buffer[128];
        sprintf(buffer, "Main loop iteration %d\n", i);

        if (global_file_state.is_open) {
            int written = write(global_file_state.fd, buffer, strlen(buffer));
            global_file_state.bytes_written += written;
        }

        if (global_log_file) {
            fprintf(global_log_file, "Main: iteration %d, bytes_written=%ld\n",
                    i, global_file_state.bytes_written);
            fflush(global_log_file);
        }

        printf("Main: fd=%d, open=%d, written=%ld\n",
               global_file_state.fd, global_file_state.is_open,
               global_file_state.bytes_written);

        usleep(150000);
    }

    if (global_file_state.fd >= 0) {
        close(global_file_state.fd);
    }
    if (global_log_file) {
        fclose(global_log_file);
    }

    return 0;
}