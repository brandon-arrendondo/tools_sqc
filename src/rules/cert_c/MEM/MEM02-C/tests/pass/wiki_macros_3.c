/*
 * Rule: MEM02-C
 * Source: wiki
 * Status: PASS - Should NOT trigger MEM02-C violation
 */

#define MALLOC_ARRAY(number, type) \
    ((type *)malloc((number) * sizeof(type)))