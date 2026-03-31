/*
 * Rule: STR04-C
 * Source: testcases
 * Status: PASS - Should NOT trigger STR04-C violation
 * Description: unsigned char used for binary data (no string literal)
 */

#include <string.h>

void binary_buffer(void) {
    unsigned char hash[32];
    unsigned char key[16];

    memset(hash, 0, sizeof(hash));
    memset(key, 0xFF, sizeof(key));
    memcpy(hash, key, sizeof(key));
}
