/*
 * Rule: INT15-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT15-C violation
 * Description: Using strtoumax and uintmax_t for input
 */

#include <stdio.h>
#include <inttypes.h>
#include <errno.h>
#include <stdint.h>

typedef unsigned long long mytypedef_t;
#define MYTYPEDEF_MAX ULLONG_MAX

void compliant(void) {
    mytypedef_t x;
    uintmax_t temp;
    char buff[256];
    char *end_ptr;

    if (fgets(buff, sizeof(buff), stdin) == NULL) {
        /* Handle error */
    } else {
        errno = 0;
        temp = strtoumax(buff, &end_ptr, 10);
        if (ERANGE == errno) {
            /* Handle error */
        } else if (end_ptr == buff) {
            /* Handle error */
        } else if ('\n' != *end_ptr && '\0' != *end_ptr) {
            /* Handle error */
        }
        if (temp > MYTYPEDEF_MAX) {
            /* Handle error */
        } else {
            x = temp;
        }
    }
}
