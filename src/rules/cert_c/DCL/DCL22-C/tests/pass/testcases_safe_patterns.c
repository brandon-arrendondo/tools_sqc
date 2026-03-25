/* Rule: DCL22-C
 * Source: testcases
 * Status: PASS - Proper volatile usage for shared/signal data
 */

#include <signal.h>
#include <stdint.h>

/* Case 1: volatile sig_atomic_t (correct) */
volatile sig_atomic_t flag = 0;

void handler(int signum) {
    flag = 1;
}

/* Case 2: volatile sig_atomic_t for shutdown flag */
volatile sig_atomic_t shutdown_requested = 0;

void sigterm_handler(int sig) {
    shutdown_requested = 1;
}

/* Case 3: volatile variable modified around external call */
extern void external_process(void);

void test_volatile_register(void) {
    volatile int32_t reg[4];

    reg[0] = 0xFF;
    external_process();
    reg[0] = 0x00;
}

/* Case 4: Plain local variable not needing volatile (no external calls between writes) */
void test_local_no_call(void) {
    int32_t counter;

    counter = 0;
    counter = 1;
    counter = 2;
}
