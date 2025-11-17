/*
 * Rule: PRE09-C
 * Source: wiki
 * Status: FAIL - Should trigger PRE09-C violation
 */

#define vsnprintf(buf, size, fmt, list) \
vsprintf(buf, fmt, list)