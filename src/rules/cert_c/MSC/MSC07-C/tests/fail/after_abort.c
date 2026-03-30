// Test: code after abort() is unreachable
#include <stdlib.h>

void handle_fatal(void) {
    abort();
    int x = 5;  // MSC07-C violation
}
