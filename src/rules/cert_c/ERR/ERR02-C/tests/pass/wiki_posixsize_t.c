/*
 * Rule: ERR02-C
 * Source: wiki
 * Status: PASS - Should NOT trigger ERR02-C violation
 */

errno_t read(int fildes, void *buf, size_t nbyte, size_t* rbytes);