// SIG34-C: Compliant - don't call signal() from signal handler
#include <signal.h>

void handler(int signum) {
    // OK: No signal() call in handler
    // Just set a flag or do minimal work
}

void test_sig34c_pass() {
    signal(SIGINT, handler);
}
