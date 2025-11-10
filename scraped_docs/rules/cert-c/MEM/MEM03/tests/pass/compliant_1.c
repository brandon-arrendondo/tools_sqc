char *secret;
/* Initialize secret to a null-terminated byte string, 
   of less than SIZE_MAX chars */

size_t size = strlen(secret);
char *new_secret;
/* Use calloc() to zero-out allocated space */
new_secret = (char *)calloc(size+1, sizeof(char));
if (!new_secret) {
  /* Handle error */
}
strcpy(new_secret, secret);

/* Process new_secret... */

/* Sanitize memory */
memset_s(new_secret, '\0', size);
free(new_secret);
new_secret = NULL;