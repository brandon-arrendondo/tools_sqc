/*
 * Rule: INT31-C
 * Source: custom
 * Status: PASS - Should NOT trigger INT31-C violation
 * Description: Bitmask limits value to target range — safe
 */

#include <stdint.h>

void func(uint32_t val) {
    uint8_t low_byte = val & 0xFF;  /* Compliant: masked to 8 bits */
    (void)low_byte;
}
