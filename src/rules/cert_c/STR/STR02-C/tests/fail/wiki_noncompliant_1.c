/*
 * Rule: STR02-C
 * Source: wiki
 * Status: FAIL - Should trigger STR02-C violation
 */

sprintf(buffer, "/bin/mail %s < /tmp/email", addr);
system(buffer);