/*
 * Rule: INT30-C
 * Source: testcases
 * Status: FAIL - Should trigger INT30-C violation
 */

/*
 * Rule: INT30-C - Ensure that unsigned integer operations do not wrap
 * Status: FAIL
 * Reason: Multiplication wrap in hash table allocation
 */

#include <stdlib.h>
#include <stddef.h>

struct entry {
    int key;
    void *value;
};

void create_hash_table(size_t num_buckets) {
    // Multiplication may wrap
    struct entry *table = malloc(num_buckets * sizeof(struct entry));  // Line 17 - VIOLATION

    if (table) {
        free(table);
    }
}

int main(void) {
    create_hash_table(SIZE_MAX / 4);  // Will wrap
    return 0;
}
