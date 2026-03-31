/*
 * Rule: WIN04-C
 * Source: testcases
 * Status: PASS - Should NOT trigger WIN04-C violation
 *
 * Function pointer encrypted with EncodePointer
 */

#include <windows.h>

typedef void (*CALLBACK_FN)(void);

void register_callback_safe(CALLBACK_FN fn) {
    /* COMPLIANT: function pointer encrypted */
    CALLBACK_FN encoded = (CALLBACK_FN)EncodePointer(fn);
    CALLBACK_FN decoded = (CALLBACK_FN)DecodePointer(encoded);
    decoded();
}
