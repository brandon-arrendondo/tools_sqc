#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
char *buffer = NULL;
size_t size1;
size_t size2;
FILE *filedes;

/* Assume size1 is appropriately initialized */

filedes = fopen("out.txt", "w+");
if (filedes == NULL){
  /* Handle error */
}

buffer = (char *)calloc( 1, size1);
if (buffer == NULL) {
  /* Handle error */
}

/*
 * Accept characters in to the buffer.
 * Check for buffer overflow.
 */

size2 = strlen(buffer) + 1;

fwrite(buffer, 1, size2, filedes);

free(buffer);
buffer = NULL;
fclose(filedes);