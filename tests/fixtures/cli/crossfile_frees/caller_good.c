/*
 * Cross-file frees_params test — good caller (no leak).
 * Allocates memory and passes it to reclaim_buffer() defined in cleanup.c.
 * With -d, prescan knows reclaim_buffer frees param 0, so MEM31-C
 * should NOT flag a memory leak here.
 */

#include <stdlib.h>

void reclaim_buffer(void *buf);

void good_caller(void) {
    char *data = (char *)malloc(100);
    if (data == NULL) return;
    data[0] = 'A';
    reclaim_buffer(data);
}
