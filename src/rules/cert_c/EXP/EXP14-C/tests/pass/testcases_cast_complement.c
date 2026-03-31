/*
 * Rule: EXP14-C
 * Source: testcases
 * Status: PASS - Should NOT trigger EXP14-C violation
 * Description: Bitwise NOT with explicit cast back to small type
 */

#include <stdint.h>

void safe_complement(void) {
    uint8_t mask = 0x0F;
    uint8_t result = (uint8_t)(~mask) >> 4;  /* Cast truncates promotion */

    uint8_t val = 0xAA;
    uint8_t inverted = (uint8_t)~val;  /* Explicit cast */
}
