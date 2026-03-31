// Test: code after conditional return is reachable
#include <stdio.h>

int foo(int x) {
    if (x > 0) {
        return x;
    }
    printf("negative path\n");  // reachable when x <= 0
    return -1;
}
