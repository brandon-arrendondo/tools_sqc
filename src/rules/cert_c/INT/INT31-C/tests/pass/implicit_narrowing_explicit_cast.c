/*
 * Rule: INT31-C
 * Source: custom
 * Status: PASS - Should NOT trigger INT31-C violation
 * Description: Same-width or narrower RHS — no implicit narrowing
 */

#include <stdint.h>

void func(uint8_t a, uint16_t b) {
    uint16_t x = a;    /* Compliant: widening (8 → 16), not narrowing */
    uint32_t y = b;    /* Compliant: widening (16 → 32), not narrowing */
    uint8_t z = a;     /* Compliant: same width (8 → 8) */
    (void)x;
    (void)y;
    (void)z;
}
