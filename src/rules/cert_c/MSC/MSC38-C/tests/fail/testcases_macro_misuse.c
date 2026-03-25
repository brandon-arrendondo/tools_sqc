/*
 * Rule: MSC38-C
 * Source: testcases
 * Status: FAIL - Protected macro identifiers treated as objects
 */

#include <assert.h>
#include <errno.h>

/* #undef of assert is undefined behavior */
#undef assert

/* #undef of errno is undefined behavior */
#undef errno

/* #undef of setjmp */
#undef setjmp

/* Manual extern declaration of errno */
extern int errno;
