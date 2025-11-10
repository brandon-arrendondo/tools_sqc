#include <stdio.h>
#include <stdlib.h>
char *buffer = NULL;
size_t size1;
size_t size2;
FILE *filedes;

/* Assume size1 and size2 are appropriately initialized */

filedes = fopen("out.txt", "w+");
if (filedes == NULL) {
  /* Handle error */
}

buffer = (char *)calloc( 1, size1);
if (buffer == NULL) {
  /* Handle error */
}

fwrite(buffer, 1, size2, filedes);

free(buffer);
buffer = NULL;
fclose(filedes);