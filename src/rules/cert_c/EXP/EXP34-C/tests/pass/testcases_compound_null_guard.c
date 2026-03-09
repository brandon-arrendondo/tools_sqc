/**
 * Compliant patterns using compound null guards (|| conditions) that should NOT
 * trigger EXP34-C.
 *
 * Tests that parse_all_null_conditions collects ALL variables from compound
 * OR conditions, so all params are marked NotNull on the false branch.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/* Pattern 1: compound || guard with early return covers all pointer params */
int write_file(const char *filepath, const uint8_t *buf, size_t buflen)
{
    if (NULL == filepath || NULL == buf || 0 == buflen) {
        return 0;
    }
    /* Both filepath and buf are guaranteed non-null here */
    FILE *f = fopen(filepath, "wb");
    if (f == NULL) return -1;
    fwrite(buf, 1, buflen, f);
    fclose(f);
    return 1;
}

/* Pattern 2: two-pointer compound || guard */
int copy_data(const char *src, char *dst, size_t n)
{
    if (src == NULL || dst == NULL) {
        return -1;
    }
    for (size_t i = 0; i < n; i++) {
        dst[i] = src[i];
    }
    return 0;
}

/* Pattern 3: reversed-style compound || guard */
void process(const int *a, const int *b, size_t len)
{
    if (NULL == a || NULL == b) {
        return;
    }
    for (size_t i = 0; i < len; i++) {
        (void)(a[i] + b[i]);
    }
}
