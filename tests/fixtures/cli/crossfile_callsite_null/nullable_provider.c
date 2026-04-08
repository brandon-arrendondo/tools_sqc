/*
 * Cross-file can_return_null test — provider.
 * Defines a function that can return NULL (wraps malloc).
 * Prescan should compute can_return_null = true for get_buffer().
 */

#include <stdlib.h>

int *get_buffer(int size) {
    return (int *)malloc(size * sizeof(int));
}
