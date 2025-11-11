/*
 * Rule: SIG31-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG31-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Global flags that are NOT sig_atomic_t - rule violation */
int debug_enabled = 0;
int verbose_logging = 0;
int maintenance_mode = 0;
int emergency_shutdown = 0;
int system_ready = 1;
int data_corruption_detected = 0;
int network_available = 1;
int backup_in_progress = 0;
int user_authenticated = 0;
int cache_enabled = 1;

void unsafe_handler(int sig) {
    /* Violation: Accessing global flags that are not volatile sig_atomic_t */

    if (sig == SIGUSR1) {
        debug_enabled = 1;
        verbose_logging = 1;
        printf("Handler: Debug mode enabled by signal %d\n", sig);
    } else if (sig == SIGUSR2) {
        emergency_shutdown = 1;
        system_ready = 0;
        maintenance_mode = 1;
        printf("Handler: Emergency shutdown initiated by signal %d\n", sig);
    } else if (sig == SIGTERM) {
        system_ready = 0;
        network_available = 0;
        cache_enabled = 0;
        printf("Handler: System shutdown by signal %d\n", sig);
    }

    /* Reading and modifying multiple flags */
    if (data_corruption_detected) {
        backup_in_progress = 1;
        cache_enabled = 0;
    }

    if (!user_authenticated && system_ready) {
        network_available = 0;
    }

    printf("Handler flags: debug=%d, verbose=%d, maint=%d, ready=%d, net=%d\n",
           debug_enabled, verbose_logging, maintenance_mode, system_ready, network_available);
}

int main() {
    printf("Demonstrating unsafe global flag access in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, unsafe_handler);
    signal(SIGUSR2, unsafe_handler);
    signal(SIGTERM, unsafe_handler);

    for (int i = 0; i < 30; i++) {
        /* Main program modifies the same flags */
        debug_enabled = (i % 5 == 0) ? 1 : 0;
        verbose_logging = (i % 3 == 0) ? 1 : 0;
        maintenance_mode = (i % 10 == 0) ? 1 : 0;
        system_ready = (i % 20 != 19) ? 1 : 0;
        network_available = (i % 7 != 0) ? 1 : 0;
        data_corruption_detected = (i % 15 == 14) ? 1 : 0;
        backup_in_progress = (i % 8 == 7) ? 1 : 0;
        user_authenticated = (i % 4 == 0) ? 1 : 0;
        cache_enabled = (i % 6 != 5) ? 1 : 0;

        /* Complex flag logic */
        if (emergency_shutdown) {
            system_ready = 0;
            network_available = 0;
            cache_enabled = 0;
        }

        printf("Main flags: debug=%d, verbose=%d, maint=%d, ready=%d, net=%d, emergency=%d\n",
               debug_enabled, verbose_logging, maintenance_mode, system_ready,
               network_available, emergency_shutdown);

        usleep(100000);

        if (emergency_shutdown) {
            printf("Emergency shutdown detected, exiting...\n");
            break;
        }
    }

    return 0;
}