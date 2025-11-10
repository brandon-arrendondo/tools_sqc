char *secret;

secret = (char *)malloc(size+1);
if (!secret) {
  /* Handle error */
}

/* Perform operations using secret... */

memset_s(secret, '\0', size+1);
free(secret);
secret = NULL;