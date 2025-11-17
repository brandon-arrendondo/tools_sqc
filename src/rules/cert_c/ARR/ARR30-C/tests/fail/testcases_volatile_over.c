/*
 * Rule: ARR30-C
 * Source: testcases
 * Status: FAIL - Should trigger ARR30-C violation
 */

/*
 * Rule: ARR30-C - Do not form or use out-of-bounds pointers or array subscripts
 * Status: FAIL
 * Reason: Volatile array accessed beyond declared bounds
 */

#include <stdio.h>

volatile int hardware_regs[8] = {0};

void access_hardware() {
    // Access beyond volatile array bounds (simulating hardware registers)
    hardware_regs[10] = 0xFF;  // Line 13 - VIOLATION
    int val = hardware_regs[12];  // Line 14 - VIOLATION
    printf("Register value: %d\n", val);
}

int main(void) {
    access_hardware();
    hardware_regs[15] = 0x00;  // Line 20 - VIOLATION
    return 0;
}
