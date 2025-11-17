/*
 * Rule: SIG30-C
 * Source: testcases
 * Status: FAIL - Should trigger SIG30-C violation
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void random_handler(int sig) {
    // VIOLATION: srand() is not async-safe
    srand(time(NULL));  // time() is also not async-safe

    // VIOLATION: rand() is not async-safe
    int random_value = rand();
    random_value = rand() % 100;

    // VIOLATION: random() and srandom() are not async-safe
    srandom(42);
    long rand_val = random();

    // VIOLATION: drand48() family functions are not async-safe
    srand48(123);
    double d_val = drand48();
    long l_val = lrand48();
    long m_val = mrand48();

    // VIOLATION: initstate() and setstate() are not async-safe
    char state_buffer[256];
    char *old_state = initstate(1, state_buffer, 256);
    setstate(old_state);

    // VIOLATION: arc4random functions may not be async-safe on all systems
#ifdef __has_builtin
#if __has_builtin(__builtin_arc4random)
    uint32_t arc_val = arc4random();
    arc_val = arc4random_uniform(100);
#endif
#endif
}

int main() {
    printf("Demonstrating unsafe random number functions in signal handler\n");
    printf("PID: %d\n", getpid());

    signal(SIGUSR1, random_handler);

    printf("Send SIGUSR1 to trigger unsafe random operations\n");

    while (1) {
        pause();
    }

    return 0;
}