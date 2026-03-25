/* Rule: DCL22-C
 * Source: testcases
 * Status: FAIL - Variables should be volatile but are not
 */

#include <signal.h>
#include <stdint.h>

/* Case 1: sig_atomic_t without volatile */
sig_atomic_t flag = 0;

void handler(int signum) {
    flag = 1;
}

/* Case 2: Another sig_atomic_t without volatile */
sig_atomic_t shutdown_requested = 0;

void sigterm_handler(int sig) {
    shutdown_requested = 1;
}

/* Case 3: Variable modified around external function call */
extern void external_process(void);

void test_write_call_write(void) {
    int32_t reg[4];

    reg[0] = 0xFF;
    external_process();
    reg[0] = 0x00;
}

/* Case 4: Another write-call-write pattern */
extern void wait_for_device(void);

void test_device_register(void) {
    int32_t status;

    status = 1;
    wait_for_device();
    status = 0;
}
