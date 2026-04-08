/*
 * Cross-file frees_params test — leaky caller.
 * Allocates memory but never frees it (no call to cleanup_buffer).
 * MEM31-C should flag this as a memory leak regardless of -d.
 */

#include <stdlib.h>

void caller_leak(void) {
    char *data = (char *)malloc(100);
    if (data == NULL) return;
    data[0] = 'A';
    /* Missing: cleanup_buffer(data) or free(data) */
}
