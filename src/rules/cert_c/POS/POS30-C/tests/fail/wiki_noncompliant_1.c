/*
 * Rule: POS30-C
 * Source: wiki
 * Status: FAIL - Should trigger POS30-C violation
 */

char buf[1024];
ssize_t len = readlink("/usr/bin/perl", buf, sizeof(buf));
buf[len] = '\0';