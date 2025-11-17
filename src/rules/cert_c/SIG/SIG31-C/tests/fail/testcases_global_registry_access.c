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

#define MAX_REGISTRY_ENTRIES 100

/* Violation: Accessing global registry/catalog in signal handler */
typedef struct {
    int id;
    char name[64];
    char type[32];
    void *data_ptr;
    int status;
    time_t created_time;
    time_t last_accessed;
} registry_entry_t;

typedef struct {
    registry_entry_t entries[MAX_REGISTRY_ENTRIES];
    int entry_count;
    int next_id;
    char registry_name[128];
    int total_lookups;
    int successful_lookups;
    double average_lookup_time;
} global_registry_t;

global_registry_t system_registry = {0};

void init_registry(global_registry_t *registry, const char *name) {
    registry->entry_count = 0;
    registry->next_id = 1;
    strcpy(registry->registry_name, name);
    registry->total_lookups = 0;
    registry->successful_lookups = 0;
    registry->average_lookup_time = 0.0;
}

int register_entry(global_registry_t *registry, const char *name, const char *type, void *data) {
    if (registry->entry_count >= MAX_REGISTRY_ENTRIES) {
        return -1;  /* Registry full */
    }

    registry_entry_t *entry = &registry->entries[registry->entry_count];
    entry->id = registry->next_id++;
    strcpy(entry->name, name);
    strcpy(entry->type, type);
    entry->data_ptr = data;
    entry->status = 1;  /* Active */
    entry->created_time = time(NULL);
    entry->last_accessed = entry->created_time;

    registry->entry_count++;
    return entry->id;
}

registry_entry_t* lookup_entry(global_registry_t *registry, int id) {
    registry->total_lookups++;

    for (int i = 0; i < registry->entry_count; i++) {
        if (registry->entries[i].id == id && registry->entries[i].status == 1) {
            registry->entries[i].last_accessed = time(NULL);
            registry->successful_lookups++;
            return &registry->entries[i];
        }
    }
    return NULL;
}

registry_entry_t* lookup_entry_by_name(global_registry_t *registry, const char *name) {
    registry->total_lookups++;

    for (int i = 0; i < registry->entry_count; i++) {
        if (strcmp(registry->entries[i].name, name) == 0 && registry->entries[i].status == 1) {
            registry->entries[i].last_accessed = time(NULL);
            registry->successful_lookups++;
            return &registry->entries[i];
        }
    }
    return NULL;
}

void unsafe_handler(int sig) {
    /* Violation: Accessing and modifying global registry in signal handler */
    char signal_entry_name[64];
    sprintf(signal_entry_name, "signal_handler_%d", sig);

    /* Register new entry in signal handler - dangerous */
    int *signal_data = malloc(sizeof(int));
    if (signal_data) {
        *signal_data = sig * 1000;
        register_entry(&system_registry, signal_entry_name, "signal_type", signal_data);
    }

    /* Lookup and modify existing entries */
    for (int i = 0; i < system_registry.entry_count; i++) {
        registry_entry_t *entry = &system_registry.entries[i];
        if (entry->status == 1) {
            /* Modify entry in signal handler */
            entry->status = 2;  /* Mark as signal-modified */
            sprintf(entry->type, "modified_by_%d", sig);
            entry->last_accessed = time(NULL);
        }
    }

    /* Update registry metadata */
    sprintf(system_registry.registry_name, "signal_modified_registry_%d", sig);
    system_registry.total_lookups += sig;
    system_registry.successful_lookups += sig / 2;

    /* Recalculate average lookup time */
    if (system_registry.total_lookups > 0) {
        system_registry.average_lookup_time =
            (double)system_registry.successful_lookups / system_registry.total_lookups * 0.001;
    }

    printf("Handler: entries=%d, lookups=%d, success=%d, avg_time=%.3f, signal=%d\n",
           system_registry.entry_count, system_registry.total_lookups,
           system_registry.successful_lookups, system_registry.average_lookup_time, sig);
}

int main() {
    printf("Demonstrating unsafe global registry access in signal handler\n");
    printf("PID: %d\n", getpid());

    /* Initialize registry */
    init_registry(&system_registry, "main_system_registry");

    /* Register some initial entries */
    int *data1 = malloc(sizeof(int)); *data1 = 100;
    int *data2 = malloc(sizeof(int)); *data2 = 200;
    int *data3 = malloc(sizeof(int)); *data3 = 300;

    int id1 = register_entry(&system_registry, "service_1", "network_service", data1);
    int id2 = register_entry(&system_registry, "service_2", "database_service", data2);
    int id3 = register_entry(&system_registry, "service_3", "file_service", data3);

    signal(SIGUSR1, unsafe_handler);

    for (int i = 0; i < 25; i++) {
        /* Main program also uses the registry */
        char main_entry_name[64];
        sprintf(main_entry_name, "main_service_%d", i);

        int *main_data = malloc(sizeof(int));
        if (main_data) {
            *main_data = i * 50;
            register_entry(&system_registry, main_entry_name, "main_service", main_data);
        }

        /* Perform lookups */
        registry_entry_t *entry = lookup_entry(&system_registry, id1);
        if (entry) {
            printf("Main: Found entry '%s' with type '%s'\n", entry->name, entry->type);
        }

        /* Lookup by name */
        entry = lookup_entry_by_name(&system_registry, "service_2");
        if (entry) {
            entry->status = 1;  /* Ensure it stays active */
        }

        /* Update registry name periodically */
        if (i % 6 == 5) {
            sprintf(system_registry.registry_name, "main_updated_registry_%d", i);
        }

        /* Calculate success rate */
        double success_rate = 0.0;
        if (system_registry.total_lookups > 0) {
            success_rate = (double)system_registry.successful_lookups /
                          system_registry.total_lookups * 100.0;
        }

        printf("Main[%d]: entries=%d, lookups=%d, success_rate=%.1f%%, name=%s\n",
               i, system_registry.entry_count, system_registry.total_lookups,
               success_rate, system_registry.registry_name);

        usleep(120000);
    }

    /* Cleanup registry */
    for (int i = 0; i < system_registry.entry_count; i++) {
        if (system_registry.entries[i].data_ptr) {
            free(system_registry.entries[i].data_ptr);
        }
    }

    printf("Final registry state: %d entries, %d total lookups, %.1f%% success rate\n",
           system_registry.entry_count, system_registry.total_lookups,
           system_registry.total_lookups > 0 ?
           (double)system_registry.successful_lookups / system_registry.total_lookups * 100.0 : 0.0);

    return 0;
}