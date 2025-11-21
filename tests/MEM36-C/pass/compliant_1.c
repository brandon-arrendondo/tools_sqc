// MEM36-C: Compliant - don't realloc aligned memory
#include <stdlib.h>

void test_mem36c_pass() {
    void *ptr = aligned_alloc(16, 1024);  // Aligned allocation
    if (ptr) {
        // OK: Free aligned memory, then allocate new if needed
        free(ptr);
        ptr = malloc(2048);
        free(ptr);
    }
}
