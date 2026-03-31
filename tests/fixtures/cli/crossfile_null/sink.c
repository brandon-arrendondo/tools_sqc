/*
 * Cross-file global null test — sink file.
 * Declares extern global and dereferences without null check.
 * With -d pointing to source.c's directory, prescan should detect
 * shared_buffer = NULL and flag the dereference.
 */

#include <stdio.h>

extern int *shared_buffer;

void bad_sink(void) {
    /* Dereference of cross-file global that is NULL */
    *shared_buffer = 42;
    printf("Value: %d\n", *shared_buffer);
}
