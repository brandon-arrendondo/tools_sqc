/*
 * Cross-file global null test — safe sink file.
 * Declares extern global but checks for NULL before dereference.
 * Should produce no EXP34-C violations even with -d flag.
 */

#include <stdio.h>

extern int *shared_buffer;

void good_sink(void) {
    if (shared_buffer != NULL) {
        *shared_buffer = 42;
        printf("Value: %d\n", *shared_buffer);
    }
}
