/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Shared counters and statistics not properly protected */
int request_counter = 0;
int error_counter = 0;
long total_bytes_processed = 0;
double average_processing_time = 0.0;
int peak_memory_usage = 0;

void unsafe_handler(int sig) {
    /* Violation: Accessing shared counters in signal handler */
    request_counter++;
    if (sig == SIGUSR2) {
        error_counter++;
    }

    total_bytes_processed += 1024;
    average_processing_time = (average_processing_time + 0.5) / 2.0;
    peak_memory_usage = request_counter * 4096;

    printf("Handler: requests=%d, errors=%d, bytes=%ld, avg_time=%.2f, peak_mem=%d\n",
           request_counter, error_counter, total_bytes_processed,
           average_processing_time, peak_memory_usage);
}

int main() {
    printf("Demonstrating unsafe shared counter access in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, unsafe_handler);
    signal(SIGUSR2, unsafe_handler);

    for (int i = 0; i < 40; i++) {
        request_counter = i * 2;
        error_counter = i / 5;
        total_bytes_processed = i * 512;
        average_processing_time = i * 0.1;
        peak_memory_usage = i * 2048;

        printf("Main: requests=%d, errors=%d, bytes=%ld, avg_time=%.2f, peak_mem=%d\n",
               request_counter, error_counter, total_bytes_processed,
               average_processing_time, peak_memory_usage);

        usleep(75000);
    }

    return 0;
}