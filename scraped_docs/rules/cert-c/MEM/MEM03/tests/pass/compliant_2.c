char *secret;

/* Initialize secret */

size_t secret_size = strlen(secret);
char *temp_buff;
/* ... */
if (secret_size > SIZE_MAX/2) {
   /* Handle error condition */
}
/* calloc() initializes memory to zero */
temp_buff = (char *)calloc(secret_size * 2, sizeof(char));
if (temp_buff == NULL) {
 /* Handle error */
}

memcpy(temp_buff, secret, secret_size);

/* Sanitize the buffer */
memset((volatile char *)secret, '\0', secret_size);

free(secret);
secret = temp_buff; /* Install the resized buffer */
temp_buff = NULL;