/*
 * Cross-file callsite null test — callee file.
 * Defines a function that dereferences its pointer parameter.
 * The prescan should compute dereferences_params = {0} for process_data().
 */

#include <stdio.h>

void process_data(int *ptr) {
    /* Dereference of pointer parameter */
    *ptr = 42;
    printf("Value: %d\n", *ptr);
}
