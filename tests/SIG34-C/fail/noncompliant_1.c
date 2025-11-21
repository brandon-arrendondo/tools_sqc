// SIG34-C: Noncompliant - call signal() from signal handler
#include <signal.h>

void handler(int signum) {
    signal(SIGINT, handler);  // VIOLATION: signal() call in handler
}

void test_sig34c_fail() {
    signal(SIGINT, handler);
}
