// MEM36-C: Noncompliant - realloc on aligned_alloc pointer
#include <stdlib.h>

void test_mem36c_fail() {
    void *ptr = aligned_alloc(16, 1024);  // Aligned allocation
    if (ptr) {
        ptr = realloc(ptr, 2048);  // VIOLATION: realloc on aligned memory
        free(ptr);
    }
}
