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

#define POOL_SIZE 20

typedef struct resource {
    int id;
    int in_use;
    char owner[64];
    double usage_time;
    struct resource *next_free;
} resource_t;

typedef struct {
    resource_t resources[POOL_SIZE];
    resource_t *free_list;
    int allocated_count;
    int peak_usage;
    char pool_name[128];
} resource_pool_t;

resource_pool_t global_memory_pool = {0};
resource_pool_t global_connection_pool = {0};

void init_pool(resource_pool_t *pool, const char *name) {
    strcpy(pool->pool_name, name);
    pool->free_list = NULL;
    pool->allocated_count = 0;
    pool->peak_usage = 0;

    /* Initialize free list */
    for (int i = 0; i < POOL_SIZE; i++) {
        pool->resources[i].id = i;
        pool->resources[i].in_use = 0;
        pool->resources[i].owner[0] = '\0';
        pool->resources[i].usage_time = 0.0;
        pool->resources[i].next_free = pool->free_list;
        pool->free_list = &pool->resources[i];
    }
}

void unsafe_handler(int sig) {
    /* Violation: Accessing shared resource pools in signal handler */

    /* Try to allocate from memory pool */
    if (global_memory_pool.free_list) {
        resource_t *res = global_memory_pool.free_list;
        global_memory_pool.free_list = res->next_free;
        res->in_use = 1;
        sprintf(res->owner, "signal_handler_%d", sig);
        res->usage_time = 0.5;
        global_memory_pool.allocated_count++;
        if (global_memory_pool.allocated_count > global_memory_pool.peak_usage) {
            global_memory_pool.peak_usage = global_memory_pool.allocated_count;
        }
    }

    /* Modify connection pool stats */
    global_connection_pool.allocated_count += sig % 3;
    if (global_connection_pool.allocated_count > global_connection_pool.peak_usage) {
        global_connection_pool.peak_usage = global_connection_pool.allocated_count;
    }

    printf("Handler: mem_pool(alloc=%d, peak=%d), conn_pool(alloc=%d, peak=%d)\n",
           global_memory_pool.allocated_count, global_memory_pool.peak_usage,
           global_connection_pool.allocated_count, global_connection_pool.peak_usage);
}

int main() {
    printf("Demonstrating unsafe resource pool access in signal handler\n");
    printf("PID: %d\n", getpid());

    init_pool(&global_memory_pool, "Memory Pool");
    init_pool(&global_connection_pool, "Connection Pool");

    signal(SIGUSR1, unsafe_handler);

    for (int i = 0; i < 25; i++) {
        /* Allocate from memory pool */
        if (global_memory_pool.free_list) {
            resource_t *res = global_memory_pool.free_list;
            global_memory_pool.free_list = res->next_free;
            res->in_use = 1;
            sprintf(res->owner, "main_thread_%d", i);
            res->usage_time = i * 0.1;
            global_memory_pool.allocated_count++;
            if (global_memory_pool.allocated_count > global_memory_pool.peak_usage) {
                global_memory_pool.peak_usage = global_memory_pool.allocated_count;
            }
        }

        /* Update connection pool */
        global_connection_pool.allocated_count = (i % 10) + 1;
        if (global_connection_pool.allocated_count > global_connection_pool.peak_usage) {
            global_connection_pool.peak_usage = global_connection_pool.allocated_count;
        }

        printf("Main: mem_pool(alloc=%d, peak=%d), conn_pool(alloc=%d, peak=%d)\n",
               global_memory_pool.allocated_count, global_memory_pool.peak_usage,
               global_connection_pool.allocated_count, global_connection_pool.peak_usage);

        usleep(120000);
    }

    return 0;
}