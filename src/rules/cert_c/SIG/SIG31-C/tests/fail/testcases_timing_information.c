/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

typedef struct {
    struct timeval start_time;
    struct timeval last_update;
    double elapsed_seconds;
    double average_interval;
    int timer_count;
} timer_info_t;

typedef struct {
    clock_t cpu_start;
    clock_t cpu_current;
    double cpu_usage_percent;
    long context_switches;
    char timing_log[512];
} timing_data_t;

timer_info_t global_timer = {0};
timing_data_t global_timing = {0};

void unsafe_handler(int sig) {
    /* Violation: Accessing shared timers and timing information in signal handler */

    struct timeval current_time;
    gettimeofday(&current_time, NULL);

    /* Update timer information */
    global_timer.last_update = current_time;
    global_timer.elapsed_seconds = (current_time.tv_sec - global_timer.start_time.tv_sec) +
                                   (current_time.tv_usec - global_timer.start_time.tv_usec) / 1000000.0;
    global_timer.timer_count++;

    if (global_timer.timer_count > 1) {
        global_timer.average_interval = global_timer.elapsed_seconds / global_timer.timer_count;
    }

    /* Update CPU timing */
    global_timing.cpu_current = clock();
    double cpu_time = ((double)(global_timing.cpu_current - global_timing.cpu_start)) / CLOCKS_PER_SEC;
    global_timing.cpu_usage_percent = (cpu_time / global_timer.elapsed_seconds) * 100.0;
    global_timing.context_switches += sig % 5;

    sprintf(global_timing.timing_log, "Handler: sig=%d, elapsed=%.2f, cpu=%.1f%%, switches=%ld",
            sig, global_timer.elapsed_seconds, global_timing.cpu_usage_percent,
            global_timing.context_switches);

    printf("Handler: count=%d, elapsed=%.2f, avg_interval=%.3f, cpu=%.1f%%\n",
           global_timer.timer_count, global_timer.elapsed_seconds,
           global_timer.average_interval, global_timing.cpu_usage_percent);
}

int main() {
    printf("Demonstrating unsafe timing information access in signal handler\n");
    printf("PID: %d\n", getpid());

    /* Initialize timing */
    gettimeofday(&global_timer.start_time, NULL);
    global_timer.last_update = global_timer.start_time;
    global_timing.cpu_start = clock();

    signal(SIGUSR1, unsafe_handler);

    for (int i = 0; i < 25; i++) {
        struct timeval current_time;
        gettimeofday(&current_time, NULL);

        /* Update timer information */
        global_timer.last_update = current_time;
        global_timer.elapsed_seconds = (current_time.tv_sec - global_timer.start_time.tv_sec) +
                                       (current_time.tv_usec - global_timer.start_time.tv_usec) / 1000000.0;
        global_timer.timer_count = i + 1;
        global_timer.average_interval = global_timer.elapsed_seconds / global_timer.timer_count;

        /* Update CPU timing */
        global_timing.cpu_current = clock();
        double cpu_time = ((double)(global_timing.cpu_current - global_timing.cpu_start)) / CLOCKS_PER_SEC;
        global_timing.cpu_usage_percent = (cpu_time / global_timer.elapsed_seconds) * 100.0;
        global_timing.context_switches = i * 3;

        sprintf(global_timing.timing_log, "Main: iteration=%d, elapsed=%.2f, cpu=%.1f%%, switches=%ld",
                i, global_timer.elapsed_seconds, global_timing.cpu_usage_percent,
                global_timing.context_switches);

        printf("Main: count=%d, elapsed=%.2f, avg_interval=%.3f, cpu=%.1f%%\n",
               global_timer.timer_count, global_timer.elapsed_seconds,
               global_timer.average_interval, global_timing.cpu_usage_percent);

        usleep(100000);
    }

    return 0;
}