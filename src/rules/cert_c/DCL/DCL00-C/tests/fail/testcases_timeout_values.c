/*
 * Rule: DCL00-C
 * Source: testcases
 * Status: FAIL - Should trigger DCL00-C violation
 */

/*
 * CERT C DCL00-C Fail Case: timeout_values.c
 *
 * This case demonstrates violations where timeout and timing
 * constants that never change are not const-qualified.
 */

#include <stdio.h>
#include <unistd.h>

void network_timeouts(void) {
    /* NON-COMPLIANT: Network timeout values should be const */
    int CONNECTION_TIMEOUT = 5000;     /* milliseconds */
    int READ_TIMEOUT = 3000;
    int WRITE_TIMEOUT = 3000;
    int KEEP_ALIVE_TIMEOUT = 30000;
    int DNS_TIMEOUT = 2000;

    printf("Network Timeout Configuration:\n");
    printf("  Connection: %d ms\n", CONNECTION_TIMEOUT);
    printf("  Read: %d ms\n", READ_TIMEOUT);
    printf("  Write: %d ms\n", WRITE_TIMEOUT);
    printf("  Keep-alive: %d ms\n", KEEP_ALIVE_TIMEOUT);
    printf("  DNS: %d ms\n", DNS_TIMEOUT);

    /* Timeouts used for comparison but never modified */
    int elapsed_time = 2500;
    if (elapsed_time > CONNECTION_TIMEOUT) {
        printf("Connection timed out!\n");
    } else if (elapsed_time > READ_TIMEOUT) {
        printf("Read operation timed out!\n");
    } else {
        printf("Operation completed within timeout\n");
    }
}

void retry_intervals(void) {
    /* NON-COMPLIANT: Retry timing should be const */
    int INITIAL_RETRY_DELAY = 100;     /* milliseconds */
    int MAX_RETRY_DELAY = 5000;
    int RETRY_BACKOFF_FACTOR = 2;
    int MAX_RETRY_COUNT = 3;

    /* NON-COMPLIANT: Exponential backoff intervals should be const */
    int retry_delays[] = {100, 200, 400, 800, 1600, 3200};

    printf("\nRetry Configuration:\n");
    printf("  Initial delay: %d ms\n", INITIAL_RETRY_DELAY);
    printf("  Max delay: %d ms\n", MAX_RETRY_DELAY);
    printf("  Backoff factor: %d\n", RETRY_BACKOFF_FACTOR);
    printf("  Max retries: %d\n", MAX_RETRY_COUNT);

    printf("  Retry schedule: ");
    for (int i = 0; i < 6; i++) {
        printf("%d", retry_delays[i]);
        if (i < 5) printf(", ");
    }
    printf(" ms\n");

    /* Simulate retry logic using constants */
    for (int attempt = 1; attempt <= MAX_RETRY_COUNT; attempt++) {
        int delay = retry_delays[attempt - 1];
        printf("  Attempt %d: wait %d ms\n", attempt, delay);
    }
}

void cache_expiration(void) {
    /* NON-COMPLIANT: Cache timing should be const */
    int CACHE_TTL_SHORT = 300;         /* seconds (5 minutes) */
    int CACHE_TTL_MEDIUM = 1800;       /* seconds (30 minutes) */
    int CACHE_TTL_LONG = 3600;         /* seconds (1 hour) */
    int CACHE_TTL_PERMANENT = 86400;   /* seconds (24 hours) */

    /* NON-COMPLIANT: Cleanup intervals should be const */
    int CLEANUP_INTERVAL = 600;        /* seconds (10 minutes) */
    int FULL_CLEANUP_INTERVAL = 3600;  /* seconds (1 hour) */

    printf("\nCache Expiration Settings:\n");
    printf("  Short TTL: %d seconds\n", CACHE_TTL_SHORT);
    printf("  Medium TTL: %d seconds\n", CACHE_TTL_MEDIUM);
    printf("  Long TTL: %d seconds\n", CACHE_TTL_LONG);
    printf("  Permanent TTL: %d seconds\n", CACHE_TTL_PERMANENT);

    printf("  Cleanup interval: %d seconds\n", CLEANUP_INTERVAL);
    printf("  Full cleanup: %d seconds\n", FULL_CLEANUP_INTERVAL);

    /* TTL values used for cache management but never modified */
    int item_age = 1200;  /* 20 minutes */
    printf("\nCache item age: %d seconds\n", item_age);

    if (item_age > CACHE_TTL_LONG) {
        printf("  Item expired (long TTL)\n");
    } else if (item_age > CACHE_TTL_MEDIUM) {
        printf("  Item expired (medium TTL)\n");
    } else if (item_age > CACHE_TTL_SHORT) {
        printf("  Item expired (short TTL)\n");
    } else {
        printf("  Item still valid\n");
    }
}

void polling_intervals(void) {
    /* NON-COMPLIANT: Polling intervals should be const */
    int FAST_POLL_INTERVAL = 100;      /* milliseconds */
    int NORMAL_POLL_INTERVAL = 1000;   /* milliseconds */
    int SLOW_POLL_INTERVAL = 5000;     /* milliseconds */
    int IDLE_POLL_INTERVAL = 30000;    /* milliseconds */

    /* NON-COMPLIANT: Heartbeat timing should be const */
    int HEARTBEAT_INTERVAL = 10000;    /* milliseconds */
    int HEARTBEAT_TIMEOUT = 30000;     /* milliseconds */

    printf("\nPolling Configuration:\n");
    printf("  Fast polling: %d ms\n", FAST_POLL_INTERVAL);
    printf("  Normal polling: %d ms\n", NORMAL_POLL_INTERVAL);
    printf("  Slow polling: %d ms\n", SLOW_POLL_INTERVAL);
    printf("  Idle polling: %d ms\n", IDLE_POLL_INTERVAL);

    printf("  Heartbeat interval: %d ms\n", HEARTBEAT_INTERVAL);
    printf("  Heartbeat timeout: %d ms\n", HEARTBEAT_TIMEOUT);

    /* Intervals used for scheduling but never modified */
    int system_load = 75;  /* percentage */
    int poll_interval;

    if (system_load > 90) {
        poll_interval = SLOW_POLL_INTERVAL;
    } else if (system_load > 50) {
        poll_interval = NORMAL_POLL_INTERVAL;
    } else {
        poll_interval = FAST_POLL_INTERVAL;
    }

    printf("  Selected interval for load %d%%: %d ms\n", system_load, poll_interval);
}

int main(void) {
    /* NON-COMPLIANT: Process timing should be const */
    int STARTUP_TIMEOUT = 10000;       /* milliseconds */
    int SHUTDOWN_TIMEOUT = 5000;       /* milliseconds */
    int WATCHDOG_TIMEOUT = 15000;      /* milliseconds */

    printf("Process Timing Configuration:\n");
    printf("  Startup timeout: %d ms\n", STARTUP_TIMEOUT);
    printf("  Shutdown timeout: %d ms\n", SHUTDOWN_TIMEOUT);
    printf("  Watchdog timeout: %d ms\n", WATCHDOG_TIMEOUT);

    network_timeouts();
    retry_intervals();
    cache_expiration();
    polling_intervals();

    return 0;
}