/*
 * Rule: MEM03-C
 * Source: wiki
 * Status: FAIL - Should trigger MEM03-C violation
 */

char *secret;

/* Initialize secret */

size_t secret_size = strlen(secret);
/* ... */
if (secret_size > SIZE_MAX/2) {
   /* Handle error condition */
}
else {
secret = (char *)realloc(secret, secret_size * 2);
}