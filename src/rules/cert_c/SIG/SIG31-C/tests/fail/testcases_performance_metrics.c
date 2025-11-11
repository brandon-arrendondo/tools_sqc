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
#include <sys/time.h>
#include <sys/resource.h>

typedef struct {
    long requests_per_second;
    double average_response_time;
    long peak_memory_usage;
    double cpu_utilization;
    int active_threads;
    long total_requests;
    long failed_requests;
    double throughput_mbps;
} performance_metrics_t;

typedef struct {
    int cache_hits;
    int cache_misses;
    double cache_hit_ratio;
    long memory_allocations;
    long memory_deallocations;
    double memory_fragmentation;
    int gc_collections;
    double gc_time_total;
} system_counters_t;

performance_metrics_t global_perf_metrics = {0};
system_counters_t global_sys_counters = {0};

void unsafe_handler(int sig) {
    /* Violation: Accessing global performance metrics and counters in signal handler */

    /* Update performance metrics based on signal */
    if (sig == SIGUSR1) {
        global_perf_metrics.requests_per_second += 1000;
        global_perf_metrics.cpu_utilization += 10.0;
        global_perf_metrics.active_threads += 5;
        global_perf_metrics.failed_requests += sig % 10;
    } else if (sig == SIGUSR2) {
        global_perf_metrics.average_response_time += 0.5;
        global_perf_metrics.peak_memory_usage += 1024 * 1024;  /* 1MB */
        global_perf_metrics.throughput_mbps -= 0.1;
    }

    global_perf_metrics.total_requests += sig * 100;

    /* Update system counters */
    global_sys_counters.cache_misses += sig % 5;
    global_sys_counters.memory_allocations += sig * 10;
    global_sys_counters.memory_fragmentation += 0.01;
    global_sys_counters.gc_collections++;
    global_sys_counters.gc_time_total += 0.02;

    /* Recalculate derived metrics */
    if (global_sys_counters.cache_hits + global_sys_counters.cache_misses > 0) {
        global_sys_counters.cache_hit_ratio =
            (double)global_sys_counters.cache_hits /
            (global_sys_counters.cache_hits + global_sys_counters.cache_misses);
    }

    printf("Handler: rps=%ld, avg_resp=%.2f, peak_mem=%ld, cpu=%.1f%%, threads=%d\n",
           global_perf_metrics.requests_per_second,
           global_perf_metrics.average_response_time,
           global_perf_metrics.peak_memory_usage,
           global_perf_metrics.cpu_utilization,
           global_perf_metrics.active_threads);
}

int main() {
    printf("Demonstrating unsafe performance metrics access in signal handler\n");
    printf("PID: %d\n", getpid());

    /* Initialize metrics */
    global_perf_metrics.requests_per_second = 100;
    global_perf_metrics.average_response_time = 0.1;
    global_perf_metrics.peak_memory_usage = 10 * 1024 * 1024;  /* 10MB */
    global_perf_metrics.cpu_utilization = 5.0;
    global_perf_metrics.active_threads = 10;
    global_perf_metrics.throughput_mbps = 100.0;

    global_sys_counters.cache_hits = 1000;
    global_sys_counters.cache_misses = 100;
    global_sys_counters.memory_allocations = 5000;
    global_sys_counters.memory_deallocations = 4900;
    global_sys_counters.memory_fragmentation = 0.05;

    signal(SIGUSR1, unsafe_handler);
    signal(SIGUSR2, unsafe_handler);

    for (int i = 0; i < 25; i++) {
        /* Simulate performance updates */
        global_perf_metrics.requests_per_second = 100 + i * 10;
        global_perf_metrics.average_response_time = 0.1 + (i * 0.01);
        global_perf_metrics.cpu_utilization = 5.0 + (i % 20);
        global_perf_metrics.active_threads = 10 + (i % 5);
        global_perf_metrics.total_requests += i * 50;
        global_perf_metrics.failed_requests += (i % 10 == 9) ? 1 : 0;

        if (i % 3 == 0) {
            global_perf_metrics.peak_memory_usage += 1024 * 512;  /* 512KB */
        }
        if (i % 4 == 0) {
            global_perf_metrics.throughput_mbps += 0.5;
        }

        /* Update system counters */
        global_sys_counters.cache_hits += 50 + (i % 20);
        global_sys_counters.cache_misses += (i % 8 == 7) ? 5 : 1;
        global_sys_counters.memory_allocations += i * 2;
        global_sys_counters.memory_deallocations += (i * 2) - 1;

        /* Trigger garbage collection occasionally */
        if (i % 7 == 6) {
            global_sys_counters.gc_collections++;
            global_sys_counters.gc_time_total += 0.05;
            global_sys_counters.memory_fragmentation -= 0.01;
        }

        /* Recalculate cache hit ratio */
        global_sys_counters.cache_hit_ratio =
            (double)global_sys_counters.cache_hits /
            (global_sys_counters.cache_hits + global_sys_counters.cache_misses);

        printf("Main: rps=%ld, total=%ld, failed=%ld, hit_ratio=%.3f, gc=%d, frag=%.3f\n",
               global_perf_metrics.requests_per_second,
               global_perf_metrics.total_requests,
               global_perf_metrics.failed_requests,
               global_sys_counters.cache_hit_ratio,
               global_sys_counters.gc_collections,
               global_sys_counters.memory_fragmentation);

        usleep(120000);
    }

    return 0;
}