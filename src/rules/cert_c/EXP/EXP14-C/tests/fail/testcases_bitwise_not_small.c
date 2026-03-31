/*
 * Rule: EXP14-C
 * Source: testcases
 * Status: FAIL - Should trigger EXP14-C violation
 * Description: Bitwise NOT on small type without cast
 */

#include <stdint.h>

void complement_operations(void) {
    uint8_t mask = 0x0F;
    uint8_t result = (~mask) >> 4;  /* Violation: ~ promotes to int */

    uint8_t val = 0xAA;
    uint8_t shifted = (~val) << 2;  /* Violation: ~ promotes to int */
}
