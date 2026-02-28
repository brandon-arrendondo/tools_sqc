/*
 * Rule: INT31-C
 * Source: d_lib_common FN-002
 * Status: FAIL - Should trigger INT31-C violation
 * Description: Compound assignment with widening cast narrows back to uint8_t
 */

#include <stdint.h>

void parse_tag(const uint8_t *buffer) {
    uint8_t tag = 0;
    tag |= (uint16_t)(buffer[0]);  /* Violation: uint16_t → uint8_t via |= */
    (void)tag;
}
