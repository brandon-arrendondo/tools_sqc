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

#define HASH_SIZE 32

/* Violation: Accessing shared hash table in signal handler */
typedef struct hash_entry {
    char key[64];
    int value;
    struct hash_entry *next;
} hash_entry_t;

typedef struct {
    hash_entry_t *buckets[HASH_SIZE];
    int total_entries;
    int collision_count;
    char table_name[128];
    double load_factor;
} hash_table_t;

hash_table_t global_hash_table = {0};

unsigned int hash_function(const char *key) {
    unsigned int hash = 0;
    while (*key) {
        hash = hash * 31 + *key++;
    }
    return hash % HASH_SIZE;
}

void hash_insert(hash_table_t *table, const char *key, int value) {
    unsigned int index = hash_function(key);
    hash_entry_t *new_entry = malloc(sizeof(hash_entry_t));
    if (!new_entry) return;

    strcpy(new_entry->key, key);
    new_entry->value = value;

    if (table->buckets[index] != NULL) {
        table->collision_count++;
    }

    new_entry->next = table->buckets[index];
    table->buckets[index] = new_entry;
    table->total_entries++;

    table->load_factor = (double)table->total_entries / HASH_SIZE;
}

hash_entry_t* hash_lookup(hash_table_t *table, const char *key) {
    unsigned int index = hash_function(key);
    hash_entry_t *entry = table->buckets[index];

    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

void unsafe_handler(int sig) {
    /* Violation: Accessing and modifying shared hash table in signal handler */
    char signal_key[64];
    sprintf(signal_key, "signal_%d", sig);

    /* Insert new entry in signal handler - dangerous */
    hash_insert(&global_hash_table, signal_key, sig * 1000);

    /* Modify existing entries */
    for (int i = 0; i < HASH_SIZE; i++) {
        hash_entry_t *entry = global_hash_table.buckets[i];
        while (entry) {
            entry->value += sig;
            entry = entry->next;
        }
    }

    /* Update table metadata */
    sprintf(global_hash_table.table_name, "modified_by_signal_%d", sig);
    global_hash_table.collision_count += sig % 5;

    printf("Handler: entries=%d, collisions=%d, load=%.2f, signal=%d\n",
           global_hash_table.total_entries,
           global_hash_table.collision_count,
           global_hash_table.load_factor, sig);
}

int main() {
    printf("Demonstrating unsafe shared hash table access in signal handler\n");
    printf("PID: %d\n", getpid());

    /* Initialize hash table */
    strcpy(global_hash_table.table_name, "main_hash_table");

    /* Add initial entries */
    hash_insert(&global_hash_table, "init_1", 100);
    hash_insert(&global_hash_table, "init_2", 200);
    hash_insert(&global_hash_table, "init_3", 300);

    signal(SIGUSR1, unsafe_handler);

    for (int i = 0; i < 25; i++) {
        /* Main program also modifies hash table */
        char main_key[64];
        sprintf(main_key, "main_entry_%d", i);
        hash_insert(&global_hash_table, main_key, i * 50);

        /* Lookup and modify some entries */
        if (i % 3 == 0) {
            hash_entry_t *entry = hash_lookup(&global_hash_table, "init_1");
            if (entry) {
                entry->value += i;
            }
        }

        /* Update table name periodically */
        if (i % 5 == 4) {
            sprintf(global_hash_table.table_name, "main_updated_%d", i);
        }

        /* Display statistics */
        printf("Main[%d]: entries=%d, collisions=%d, load=%.2f, name=%s\n",
               i, global_hash_table.total_entries,
               global_hash_table.collision_count,
               global_hash_table.load_factor,
               global_hash_table.table_name);

        usleep(100000);
    }

    /* Cleanup hash table */
    for (int i = 0; i < HASH_SIZE; i++) {
        hash_entry_t *entry = global_hash_table.buckets[i];
        while (entry) {
            hash_entry_t *temp = entry;
            entry = entry->next;
            free(temp);
        }
    }

    return 0;
}