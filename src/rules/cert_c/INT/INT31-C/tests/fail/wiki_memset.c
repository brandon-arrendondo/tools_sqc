/*
 * Rule: INT31-C
 * Source: wiki
 * Status: FAIL - Should trigger INT31-C violation
 * Description: memset with value > UCHAR_MAX gets truncated
 */

#include <string.h>
#include <stddef.h>

int *init_memory(int *array, size_t n) {
    return memset(array, 4096, n);  /* Violation: 4096 truncated to 0 */
}
