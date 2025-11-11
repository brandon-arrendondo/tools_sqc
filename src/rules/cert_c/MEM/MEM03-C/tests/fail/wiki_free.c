/*
 * Rule: MEM03-C
 * Source: wiki
 * Status: FAIL - Should trigger MEM03-C violation
 */

char *secret;
/* Initialize secret to a null-terminated byte string, 
   of less than SIZE_MAX chars */

size_t size = strlen(secret);
char *new_secret;
new_secret = (char *)malloc(size+1);
if (!new_secret) {
  /* Handle error */
}
strcpy(new_secret, secret);

/* Process new_secret... */

free(new_secret);
new_secret = NULL;