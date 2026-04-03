/*
 * Rule: BRULE-060
 * Status: FAIL - malloc/free in non-init functions
 */
#include <stdlib.h>

void process_data(int n) {
    int *buf = malloc(n * sizeof(int));  /* violation */
    if (buf) {
        free(buf);  /* violation */
    }
}

int handle_request(void) {
    char *msg = calloc(256, 1);  /* violation */
    if (!msg) return -1;
    free(msg);  /* violation */
    return 0;
}

void resize_buffer(void **ptr, size_t new_size) {
    *ptr = realloc(*ptr, new_size);  /* violation */
}
