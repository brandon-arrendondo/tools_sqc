/*
 * Rule: WIN04-C
 * Source: wiki
 * Status: PASS - Should NOT trigger WIN04-C violation
 */

#include <Windows.h>
 
void *log_fn = EncodePointer(printf);
/* ... */
int (*fn)(const char *, ...) = (int (*)(const char *, ...))DecodePointer(log_fn);

fn("foo");