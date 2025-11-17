/*
 * Rule: SIG02-C
 * Source: testcases
 * Status: PASS - Should NOT trigger SIG02-C violation
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>

typedef struct {
    int task_count;
    time_t last_execution;
    int maintenance_cycles;
} scheduler_state_t;

scheduler_state_t scheduler = {0, 0, 0};

// Signal handler only for emergency shutdown
void emergency_handler(int sig) {
    if (sig == SIGTERM) {
        printf("EMERGENCY: Received termination signal - shutting down scheduler\n");
        exit(0);
    }
}

void execute_scheduled_task(int task_id) {
    printf("Scheduler: Executing scheduled task %d\n", task_id);
    printf("Scheduler: Performing normal business operation\n");
    printf("Scheduler: Task %d completed successfully\n", task_id);
    scheduler.task_count++;
}

void perform_maintenance() {
    printf("Scheduler: Performing regular maintenance cycle\n");
    printf("Scheduler: Cleaning up temporary files\n");
    printf("Scheduler: Updating internal statistics\n");
    scheduler.maintenance_cycles++;
}

int main() {
    printf("Using timers and polling for regular operations, signals only for emergencies (GOOD)\n");

    // Set up signal handler only for emergency conditions
    signal(SIGTERM, emergency_handler);

    printf("Scheduler: Starting timer-based task scheduler\n");

    time_t start_time = time(NULL);
    time_t last_task_time = start_time;
    time_t last_maintenance_time = start_time;

    const int TASK_INTERVAL = 3;        // Execute task every 3 seconds
    const int MAINTENANCE_INTERVAL = 8;  // Maintenance every 8 seconds
    const int TOTAL_RUNTIME = 20;       // Run for 20 seconds

    int task_id = 1;

    while (1) {
        time_t current_time = time(NULL);

        // Check if it's time for a scheduled task
        if (current_time - last_task_time >= TASK_INTERVAL) {
            execute_scheduled_task(task_id++);
            last_task_time = current_time;
        }

        // Check if it's time for maintenance
        if (current_time - last_maintenance_time >= MAINTENANCE_INTERVAL) {
            perform_maintenance();
            last_maintenance_time = current_time;
        }

        // Check if we should exit
        if (current_time - start_time >= TOTAL_RUNTIME) {
            printf("Scheduler: Normal runtime completed\n");
            break;
        }

        // Poll interval - check every 500ms
        usleep(500000);

        // Show periodic status
        if ((current_time - start_time) % 5 == 0 && current_time != start_time) {
            printf("Scheduler: Status update - running for %ld seconds\n",
                   current_time - start_time);
            printf("Scheduler: Tasks executed: %d, Maintenance cycles: %d\n",
                   scheduler.task_count, scheduler.maintenance_cycles);
        }
    }

    printf("Scheduler: Final statistics:\n");
    printf("  - Total tasks executed: %d\n", scheduler.task_count);
    printf("  - Maintenance cycles: %d\n", scheduler.maintenance_cycles);
    printf("  - Total runtime: %ld seconds\n", time(NULL) - start_time);
    printf("Timer-based scheduling completed using proper polling mechanisms\n");

    return 0;
}