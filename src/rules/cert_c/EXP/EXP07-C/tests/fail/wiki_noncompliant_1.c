/*
 * Rule: EXP07-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP07-C violation
 */

#include <stdio.h>
/* ... */
nblocks = 1 + ((nbytes - 1) >> 9); /* BUFSIZ = 512 = 2^9 */