/*
 * Rule: INT31-C
 * Source: custom
 * Status: PASS - Should NOT trigger INT31-C violation
 * Description: Literal value fits in target type — no data loss possible
 */

#include <stdint.h>

void func(void) {
    uint8_t x = 42;       /* Compliant: 42 fits in uint8_t */
    uint8_t y = 0xFF;     /* Compliant: 255 fits in uint8_t */
    char c = 65;           /* Compliant: 65 fits in char */
    (void)x;
    (void)y;
    (void)c;
}
