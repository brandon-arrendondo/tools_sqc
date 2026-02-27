/*
 * Rule: INT31-C
 * Source: custom
 * Status: FAIL - Should trigger INT31-C violation
 * Description: Variable-to-variable implicit narrowing without bounds check
 */

#include <stdint.h>

void func(void) {
    uint16_t wide = 1000;
    uint8_t narrow = wide;  /* Violation: uint16_t → uint8_t */
    (void)narrow;
}
