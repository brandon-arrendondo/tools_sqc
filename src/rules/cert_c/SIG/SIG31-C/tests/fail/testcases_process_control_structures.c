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
#include <sys/wait.h>
#include <sys/types.h>

typedef struct {
    pid_t pid;
    char process_name[64];
    int status;
    time_t start_time;
    double cpu_time;
    int priority;
    int memory_usage;
} process_info_t;

typedef struct {
    pid_t thread_id;
    char thread_name[64];
    int thread_state;  /* 0=idle, 1=running, 2=blocked, 3=terminated */
    double thread_cpu_time;
    int thread_priority;
    char thread_context[128];
} thread_info_t;

typedef struct {
    process_info_t child_processes[10];
    int active_children;
    thread_info_t worker_threads[20];
    int active_threads;
    int process_exit_codes[10];
    char system_state[256];
    int resource_limits[5];
} process_control_t;

process_control_t global_process_control = {0};

void init_process_info(process_info_t *proc, pid_t pid, const char *name) {
    proc->pid = pid;
    strcpy(proc->process_name, name);
    proc->status = 1;  /* Running */
    proc->start_time = time(NULL);
    proc->cpu_time = 0.0;
    proc->priority = 0;
    proc->memory_usage = 1024;
}

void init_thread_info(thread_info_t *thread, pid_t tid, const char *name) {
    thread->thread_id = tid;
    strcpy(thread->thread_name, name);
    thread->thread_state = 1;  /* Running */
    thread->thread_cpu_time = 0.0;
    thread->thread_priority = 0;
    strcpy(thread->thread_context, "Normal execution");
}

void unsafe_handler(int sig) {
    /* Violation: Accessing shared process/thread control structures in signal handler */

    if (sig == SIGUSR1) {
        /* Emergency process termination */
        for (int i = 0; i < global_process_control.active_children; i++) {
            process_info_t *proc = &global_process_control.child_processes[i];
            proc->status = 3;  /* Terminated */
            proc->cpu_time += 1.0;
            sprintf(proc->process_name, "emergency_term_%d", i);
            global_process_control.process_exit_codes[i] = sig;
        }

        /* Stop all worker threads */
        for (int i = 0; i < global_process_control.active_threads; i++) {
            thread_info_t *thread = &global_process_control.worker_threads[i];
            thread->thread_state = 3;  /* Terminated */
            sprintf(thread->thread_context, "Emergency stop by signal %d", sig);
            thread->thread_cpu_time += 0.5;
        }

        strcpy(global_process_control.system_state, "EMERGENCY_SHUTDOWN");
    } else if (sig == SIGUSR2) {
        /* Change process priorities */
        for (int i = 0; i < global_process_control.active_children; i++) {
            global_process_control.child_processes[i].priority = 10;  /* High priority */
            global_process_control.child_processes[i].memory_usage *= 2;
        }

        /* Boost thread priorities */
        for (int i = 0; i < global_process_control.active_threads; i++) {
            global_process_control.worker_threads[i].thread_priority = 5;
            strcpy(global_process_control.worker_threads[i].thread_context,
                   "Priority boosted by signal");
        }

        strcpy(global_process_control.system_state, "HIGH_PRIORITY_MODE");
    }

    /* Update resource limits */
    for (int i = 0; i < 5; i++) {
        global_process_control.resource_limits[i] += sig % 100;
    }

    printf("Handler: children=%d, threads=%d, system_state=%s, sig=%d\n",
           global_process_control.active_children,
           global_process_control.active_threads,
           global_process_control.system_state, sig);
}

int main() {
    printf("Demonstrating unsafe process/thread control structure access in signal handler\n");
    printf("PID: %d\n", getpid());

    /* Initialize process control structures */
    strcpy(global_process_control.system_state, "NORMAL_OPERATION");
    global_process_control.active_children = 3;
    global_process_control.active_threads = 5;

    /* Initialize child processes */
    for (int i = 0; i < 3; i++) {
        char name[64];
        sprintf(name, "worker_process_%d", i);
        init_process_info(&global_process_control.child_processes[i], 1000 + i, name);
    }

    /* Initialize worker threads */
    for (int i = 0; i < 5; i++) {
        char name[64];
        sprintf(name, "worker_thread_%d", i);
        init_thread_info(&global_process_control.worker_threads[i], 2000 + i, name);
    }

    /* Initialize resource limits */
    global_process_control.resource_limits[0] = 1024;  /* Memory limit */
    global_process_control.resource_limits[1] = 100;   /* CPU limit */
    global_process_control.resource_limits[2] = 50;    /* File descriptor limit */
    global_process_control.resource_limits[3] = 10;    /* Process limit */
    global_process_control.resource_limits[4] = 60;    /* Time limit */

    signal(SIGUSR1, unsafe_handler);
    signal(SIGUSR2, unsafe_handler);

    for (int i = 0; i < 25; i++) {
        /* Update process information */
        for (int j = 0; j < global_process_control.active_children; j++) {
            process_info_t *proc = &global_process_control.child_processes[j];
            proc->cpu_time += 0.1;
            proc->memory_usage += 10;
            if (proc->status == 1) {  /* Still running */
                proc->priority = (i + j) % 10;
            }
        }

        /* Update thread information */
        for (int j = 0; j < global_process_control.active_threads; j++) {
            thread_info_t *thread = &global_process_control.worker_threads[j];
            thread->thread_cpu_time += 0.05;
            if (thread->thread_state == 1) {  /* Still running */
                thread->thread_priority = (i + j) % 5;
                sprintf(thread->thread_context, "Main iteration %d", i);

                /* Simulate thread state changes */
                if (i % 7 == j % 7) {
                    thread->thread_state = 2;  /* Blocked */
                } else {
                    thread->thread_state = 1;  /* Running */
                }
            }
        }

        /* Update resource limits */
        for (int j = 0; j < 5; j++) {
            global_process_control.resource_limits[j] += i % 10;
        }

        /* Update system state based on conditions */
        if (i % 10 == 9) {
            strcpy(global_process_control.system_state, "MAINTENANCE_MODE");
        } else if (i % 15 == 14) {
            strcpy(global_process_control.system_state, "HEAVY_LOAD_MODE");
        } else {
            strcpy(global_process_control.system_state, "NORMAL_OPERATION");
        }

        printf("Main: iter=%d, children=%d, threads=%d, state=%s, mem_limit=%d\n",
               i, global_process_control.active_children,
               global_process_control.active_threads,
               global_process_control.system_state,
               global_process_control.resource_limits[0]);

        usleep(120000);
    }

    return 0;
}