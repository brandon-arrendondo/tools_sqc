/*
 * Cross-file global null test — source file.
 * Defines a global pointer and assigns it NULL.
 */

#include <stdlib.h>

int *shared_buffer = NULL;

void bad_source(void) {
    shared_buffer = NULL;
}
