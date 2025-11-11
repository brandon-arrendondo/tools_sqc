/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

typedef struct {
    FILE *log_file;
    char log_filename[256];
    int log_level;
    char log_buffer[4096];
    int buffer_pos;
    int total_entries;
    int error_entries;
    char last_timestamp[32];
} logging_state_t;

logging_state_t global_log_state = {0};

void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

void unsafe_handler(int sig) {
    /* Violation: Accessing shared logging state and buffers in signal handler */

    char timestamp[32];
    get_timestamp(timestamp, sizeof(timestamp));
    strcpy(global_log_state.last_timestamp, timestamp);

    /* Dangerous: Writing to log file from signal handler */
    if (global_log_state.log_file) {
        fprintf(global_log_state.log_file, "[%s] SIGNAL: Signal %d received in handler\n",
                timestamp, sig);
        fflush(global_log_state.log_file);
    }

    /* Modifying log buffer */
    char log_entry[256];
    sprintf(log_entry, "[%s] SIGNAL_%d: Handler processing\n", timestamp, sig);

    int entry_len = strlen(log_entry);
    if (global_log_state.buffer_pos + entry_len < sizeof(global_log_state.log_buffer) - 1) {
        strcpy(global_log_state.log_buffer + global_log_state.buffer_pos, log_entry);
        global_log_state.buffer_pos += entry_len;
    }

    global_log_state.total_entries++;
    if (sig == SIGUSR2) {
        global_log_state.error_entries++;
        global_log_state.log_level = 4;  /* Error level */
    }

    printf("Handler: total_entries=%d, error_entries=%d, buffer_pos=%d, level=%d\n",
           global_log_state.total_entries, global_log_state.error_entries,
           global_log_state.buffer_pos, global_log_state.log_level);
}

int main() {
    printf("Demonstrating unsafe logging state access in signal handler\n");
    printf("PID: %d\n", getpid());

    /* Initialize logging */
    strcpy(global_log_state.log_filename, "/tmp/claude/signal_test.log");
    global_log_state.log_file = fopen(global_log_state.log_filename, "w");
    global_log_state.log_level = 2;  /* Info level */
    global_log_state.buffer_pos = 0;
    global_log_state.total_entries = 0;
    global_log_state.error_entries = 0;

    signal(SIGUSR1, unsafe_handler);
    signal(SIGUSR2, unsafe_handler);

    for (int i = 0; i < 30; i++) {
        char timestamp[32];
        get_timestamp(timestamp, sizeof(timestamp));
        strcpy(global_log_state.last_timestamp, timestamp);

        /* Main program logging */
        if (global_log_state.log_file) {
            fprintf(global_log_state.log_file, "[%s] INFO: Main loop iteration %d\n",
                    timestamp, i);
            fflush(global_log_state.log_file);
        }

        /* Add to log buffer */
        char log_entry[256];
        sprintf(log_entry, "[%s] MAIN_%d: Processing iteration\n", timestamp, i);

        int entry_len = strlen(log_entry);
        if (global_log_state.buffer_pos + entry_len < sizeof(global_log_state.log_buffer) - 1) {
            strcpy(global_log_state.log_buffer + global_log_state.buffer_pos, log_entry);
            global_log_state.buffer_pos += entry_len;
        }

        global_log_state.total_entries++;
        if (i % 7 == 6) {
            global_log_state.error_entries++;
            global_log_state.log_level = 3;  /* Warning level */
        } else {
            global_log_state.log_level = 2;  /* Info level */
        }

        /* Flush buffer periodically */
        if (i % 10 == 9) {
            global_log_state.buffer_pos = 0;
            memset(global_log_state.log_buffer, 0, sizeof(global_log_state.log_buffer));
        }

        printf("Main: total_entries=%d, error_entries=%d, buffer_pos=%d, level=%d\n",
               global_log_state.total_entries, global_log_state.error_entries,
               global_log_state.buffer_pos, global_log_state.log_level);

        usleep(100000);
    }

    if (global_log_state.log_file) {
        fclose(global_log_state.log_file);
    }

    return 0;
}