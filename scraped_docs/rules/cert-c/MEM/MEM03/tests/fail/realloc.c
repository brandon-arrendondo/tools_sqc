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