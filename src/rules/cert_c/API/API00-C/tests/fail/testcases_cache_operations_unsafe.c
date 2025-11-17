/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: cache_operations_unsafe.c
 *
 * This case demonstrates violations where cache operation functions
 * don't validate their parameters properly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Cache entry structure */
typedef struct CacheEntry {
    char *key;
    void *value;
    size_t value_size;
    time_t expiry_time;
    struct CacheEntry *next;
} CacheEntry;

/* Cache structure */
typedef struct {
    CacheEntry **buckets;
    size_t bucket_count;
    size_t total_entries;
    size_t max_entries;
} Cache;

/* NON-COMPLIANT: No validation of cache creation parameters */
Cache *create_cache(size_t bucket_count, size_t max_entries) {
    Cache *cache = malloc(sizeof(Cache));

    /* No validation of bucket_count or max_entries */
    cache->bucket_count = bucket_count;  /* Could be 0 */
    cache->buckets = calloc(bucket_count, sizeof(CacheEntry *));  /* Division by zero possible */
    cache->total_entries = 0;
    cache->max_entries = max_entries;  /* Could be 0 or excessive */

    return cache;
}

/* NON-COMPLIANT: No validation of cache insertion parameters */
void cache_put(Cache *cache, const char *key, const void *value, size_t value_size, int ttl_seconds) {
    /* No validation of any parameters */
    size_t hash = 0;
    for (const char *p = key; *p; p++) {  /* key could be NULL */
        hash = hash * 31 + *p;
    }

    size_t bucket_index = hash % cache->bucket_count;  /* cache could be NULL, bucket_count could be 0 */

    CacheEntry *entry = malloc(sizeof(CacheEntry));
    entry->key = malloc(strlen(key) + 1);  /* key could be NULL */
    strcpy(entry->key, key);

    entry->value = malloc(value_size);  /* value_size could be 0 or excessive */
    memcpy(entry->value, value, value_size);  /* value could be NULL */
    entry->value_size = value_size;

    entry->expiry_time = time(NULL) + ttl_seconds;  /* ttl_seconds could be negative */
    entry->next = cache->buckets[bucket_index];
    cache->buckets[bucket_index] = entry;
    cache->total_entries++;
}

/* NON-COMPLIANT: No validation of cache retrieval parameters */
void *cache_get(Cache *cache, const char *key, size_t *value_size) {
    /* No validation of cache or key */
    size_t hash = 0;
    for (const char *p = key; *p; p++) {  /* key could be NULL */
        hash = hash * 31 + *p;
    }

    size_t bucket_index = hash % cache->bucket_count;  /* cache could be NULL, bucket_count could be 0 */

    CacheEntry *entry = cache->buckets[bucket_index];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {  /* key could be NULL */
            if (entry->expiry_time > time(NULL)) {
                if (value_size) {
                    *value_size = entry->value_size;
                }
                return entry->value;
            }
        }
        entry = entry->next;
    }

    return NULL;
}

/* NON-COMPLIANT: No validation of cache removal parameters */
int cache_remove(Cache *cache, const char *key) {
    /* No validation of cache or key */
    size_t hash = 0;
    for (const char *p = key; *p; p++) {  /* key could be NULL */
        hash = hash * 31 + *p;
    }

    size_t bucket_index = hash % cache->bucket_count;  /* cache could be NULL, bucket_count could be 0 */

    CacheEntry **current = &cache->buckets[bucket_index];
    while (*current) {
        if (strcmp((*current)->key, key) == 0) {  /* key could be NULL */
            CacheEntry *to_remove = *current;
            *current = (*current)->next;
            free(to_remove->key);
            free(to_remove->value);
            free(to_remove);
            cache->total_entries--;
            return 1;
        }
        current = &(*current)->next;
    }

    return 0;
}

/* NON-COMPLIANT: No validation of cache statistics */
void cache_stats(Cache *cache, size_t *total_entries, size_t *bucket_count, double *load_factor) {
    /* No validation of cache or output parameters */
    *total_entries = cache->total_entries;  /* cache could be NULL */
    *bucket_count = cache->bucket_count;
    *load_factor = (double)cache->total_entries / cache->bucket_count;  /* output pointers could be NULL */
}

/* NON-COMPLIANT: No validation of cache clearing */
void cache_clear(Cache *cache) {
    /* No validation of cache */
    for (size_t i = 0; i < cache->bucket_count; i++) {  /* cache could be NULL */
        CacheEntry *entry = cache->buckets[i];
        while (entry) {
            CacheEntry *next = entry->next;
            free(entry->key);
            free(entry->value);
            free(entry);
            entry = next;
        }
        cache->buckets[i] = NULL;
    }
    cache->total_entries = 0;
}

/* NON-COMPLIANT: No validation of cache eviction */
void cache_evict_expired(Cache *cache) {
    /* No validation of cache */
    time_t current_time = time(NULL);

    for (size_t i = 0; i < cache->bucket_count; i++) {  /* cache could be NULL */
        CacheEntry **current = &cache->buckets[i];
        while (*current) {
            if ((*current)->expiry_time <= current_time) {
                CacheEntry *to_remove = *current;
                *current = (*current)->next;
                free(to_remove->key);
                free(to_remove->value);
                free(to_remove);
                cache->total_entries--;
            } else {
                current = &(*current)->next;
            }
        }
    }
}

/* NON-COMPLIANT: No validation of cache resizing */
void cache_resize(Cache *cache, size_t new_bucket_count) {
    /* No validation of cache or new_bucket_count */
    CacheEntry **old_buckets = cache->buckets;  /* cache could be NULL */
    size_t old_bucket_count = cache->bucket_count;

    cache->buckets = calloc(new_bucket_count, sizeof(CacheEntry *));  /* new_bucket_count could be 0 */
    cache->bucket_count = new_bucket_count;

    /* Rehash all entries */
    for (size_t i = 0; i < old_bucket_count; i++) {
        CacheEntry *entry = old_buckets[i];
        while (entry) {
            CacheEntry *next = entry->next;

            size_t hash = 0;
            for (const char *p = entry->key; *p; p++) {
                hash = hash * 31 + *p;
            }

            size_t new_bucket_index = hash % new_bucket_count;  /* Division by zero if new_bucket_count is 0 */
            entry->next = cache->buckets[new_bucket_index];
            cache->buckets[new_bucket_index] = entry;

            entry = next;
        }
    }

    free(old_buckets);
}

/* NON-COMPLIANT: No validation of cache serialization */
void cache_serialize(Cache *cache, const char *filename) {
    /* No validation of cache or filename */
    FILE *file = fopen(filename, "wb");  /* filename could be NULL */

    if (!file) {
        return;  /* But we already tried to open NULL filename */
    }

    fwrite(&cache->bucket_count, sizeof(size_t), 1, file);  /* cache could be NULL */
    fwrite(&cache->total_entries, sizeof(size_t), 1, file);

    for (size_t i = 0; i < cache->bucket_count; i++) {
        CacheEntry *entry = cache->buckets[i];
        while (entry) {
            size_t key_len = strlen(entry->key);
            fwrite(&key_len, sizeof(size_t), 1, file);
            fwrite(entry->key, 1, key_len, file);
            fwrite(&entry->value_size, sizeof(size_t), 1, file);
            fwrite(entry->value, 1, entry->value_size, file);
            fwrite(&entry->expiry_time, sizeof(time_t), 1, file);
            entry = entry->next;
        }
    }

    fclose(file);
}

int main(void) {
    Cache *null_cache = NULL;
    char *null_key = NULL;
    void *null_value = NULL;

    /* Examples of dangerous cache operations */
    // create_cache(0, 0);  /* Zero bucket count and max entries */
    // cache_put(null_cache, null_key, null_value, 0, -100);  /* NULL parameters */
    // cache_get(null_cache, null_key, NULL);  /* NULL parameters */
    // cache_remove(null_cache, null_key);  /* NULL parameters */
    // cache_stats(null_cache, NULL, NULL, NULL);  /* NULL parameters */
    // cache_clear(null_cache);  /* NULL cache */
    // cache_evict_expired(null_cache);  /* NULL cache */
    // cache_resize(null_cache, 0);  /* NULL cache and zero size */
    // cache_serialize(null_cache, NULL);  /* NULL parameters */

    printf("Cache functions compiled but lack parameter validation\n");
    return 0;
}