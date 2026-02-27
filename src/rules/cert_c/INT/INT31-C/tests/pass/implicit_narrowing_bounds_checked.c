/*
 * Rule: INT31-C
 * Source: custom
 * Status: PASS - Should NOT trigger INT31-C violation
 * Description: Bounds check before narrowing assignment — safe
 */

#include <stdint.h>
#include <limits.h>

void func(uint16_t val) {
    if (val <= UCHAR_MAX) {
        uint8_t x = val;  /* Compliant: bounds checked */
        (void)x;
    }
}
