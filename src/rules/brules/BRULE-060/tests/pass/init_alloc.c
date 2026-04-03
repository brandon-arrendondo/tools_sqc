/*
 * Rule: BRULE-060
 * Status: PASS - malloc/free in init functions and main
 */
#include <stdlib.h>

int main(void) {
    int *buf = malloc(100 * sizeof(int));
    if (!buf) return 1;
    free(buf);
    return 0;
}

void system_init(void) {
    void *pool = malloc(4096);
    if (pool) {
        /* use pool */
    }
}

void module_setup(void) {
    char *config = calloc(1024, 1);
    if (config) {
        /* use config */
    }
}

void *buffer_create(size_t size) {
    return malloc(size);
}

void pool_initialize(void) {
    void *p = malloc(8192);
    free(p);
}

void *alloc_context(void) {
    return calloc(1, 256);
}

void create_session(void) {
    void *s = malloc(512);
    (void)s;
}

void *new_buffer(size_t n) {
    return malloc(n);
}

void init(void) {
    void *p = malloc(64);
    (void)p;
}
