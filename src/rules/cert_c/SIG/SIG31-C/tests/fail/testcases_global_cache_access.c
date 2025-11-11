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

#define CACHE_SIZE 100

typedef struct cache_entry {
    char key[64];
    char value[256];
    int access_count;
    struct cache_entry *next;
} cache_entry_t;

typedef struct {
    cache_entry_t *entries[CACHE_SIZE];
    int total_entries;
    int cache_hits;
    int cache_misses;
    char last_accessed_key[64];
} cache_t;

cache_t global_cache = {0};

unsigned int hash(const char *key) {
    unsigned int hash_val = 0;
    while (*key) {
        hash_val = hash_val * 31 + *key++;
    }
    return hash_val % CACHE_SIZE;
}

void unsafe_handler(int sig) {
    /* Violation: Accessing global cache and lookup tables in signal handler */
    char signal_key[64];
    sprintf(signal_key, "signal_%d", sig);

    unsigned int index = hash(signal_key);
    cache_entry_t *entry = global_cache.entries[index];

    /* Search for existing entry */
    while (entry && strcmp(entry->key, signal_key) != 0) {
        entry = entry->next;
    }

    if (entry) {
        entry->access_count++;
        global_cache.cache_hits++;
    } else {
        /* Create new cache entry */
        cache_entry_t *new_entry = malloc(sizeof(cache_entry_t));
        if (new_entry) {
            strcpy(new_entry->key, signal_key);
            sprintf(new_entry->value, "Handler data for signal %d", sig);
            new_entry->access_count = 1;
            new_entry->next = global_cache.entries[index];
            global_cache.entries[index] = new_entry;
            global_cache.total_entries++;
        }
        global_cache.cache_misses++;
    }

    strcpy(global_cache.last_accessed_key, signal_key);

    printf("Handler: entries=%d, hits=%d, misses=%d, last_key=%s\n",
           global_cache.total_entries, global_cache.cache_hits,
           global_cache.cache_misses, global_cache.last_accessed_key);
}

int main() {
    printf("Demonstrating unsafe global cache access in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, unsafe_handler);

    for (int i = 0; i < 30; i++) {
        char key[64];
        sprintf(key, "main_key_%d", i);

        unsigned int index = hash(key);
        cache_entry_t *entry = global_cache.entries[index];

        /* Search for existing entry */
        while (entry && strcmp(entry->key, key) != 0) {
            entry = entry->next;
        }

        if (entry) {
            entry->access_count++;
            global_cache.cache_hits++;
        } else {
            /* Create new cache entry */
            cache_entry_t *new_entry = malloc(sizeof(cache_entry_t));
            if (new_entry) {
                strcpy(new_entry->key, key);
                sprintf(new_entry->value, "Main data for iteration %d", i);
                new_entry->access_count = 1;
                new_entry->next = global_cache.entries[index];
                global_cache.entries[index] = new_entry;
                global_cache.total_entries++;
            }
            global_cache.cache_misses++;
        }

        strcpy(global_cache.last_accessed_key, key);

        printf("Main: entries=%d, hits=%d, misses=%d, last_key=%s\n",
               global_cache.total_entries, global_cache.cache_hits,
               global_cache.cache_misses, global_cache.last_accessed_key);

        usleep(100000);
    }

    /* Cleanup cache */
    for (int i = 0; i < CACHE_SIZE; i++) {
        cache_entry_t *entry = global_cache.entries[i];
        while (entry) {
            cache_entry_t *temp = entry;
            entry = entry->next;
            free(temp);
        }
    }

    return 0;
}