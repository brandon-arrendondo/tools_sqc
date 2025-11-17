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

typedef struct {
    int max_connections;
    int timeout_seconds;
    char server_name[128];
    int debug_level;
    int enable_logging;
    double cache_size_mb;
} config_t;

config_t global_config = {
    .max_connections = 100,
    .timeout_seconds = 30,
    .server_name = "default_server",
    .debug_level = 1,
    .enable_logging = 1,
    .cache_size_mb = 64.0
};

void unsafe_handler(int sig) {
    /* Violation: Accessing global configuration in signal handler */
    if (sig == SIGUSR1) {
        global_config.debug_level = 3;  /* Emergency debug mode */
        global_config.enable_logging = 1;
        strcpy(global_config.server_name, "emergency_mode");
    } else if (sig == SIGUSR2) {
        global_config.max_connections = 50;  /* Reduce load */
        global_config.timeout_seconds = 10;
        global_config.cache_size_mb = 32.0;
    }

    printf("Handler: max_conn=%d, timeout=%d, server=%s, debug=%d, log=%d, cache=%.1f\n",
           global_config.max_connections, global_config.timeout_seconds,
           global_config.server_name, global_config.debug_level,
           global_config.enable_logging, global_config.cache_size_mb);
}

int main() {
    printf("Demonstrating unsafe global configuration access in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, unsafe_handler);
    signal(SIGUSR2, unsafe_handler);

    for (int i = 0; i < 25; i++) {
        global_config.max_connections = 100 + i;
        global_config.timeout_seconds = 30 + (i % 10);
        sprintf(global_config.server_name, "server_%d", i);
        global_config.debug_level = i % 4;
        global_config.enable_logging = i % 2;
        global_config.cache_size_mb = 64.0 + i;

        printf("Main: max_conn=%d, timeout=%d, server=%s, debug=%d, log=%d, cache=%.1f\n",
               global_config.max_connections, global_config.timeout_seconds,
               global_config.server_name, global_config.debug_level,
               global_config.enable_logging, global_config.cache_size_mb);

        usleep(120000);
    }

    return 0;
}