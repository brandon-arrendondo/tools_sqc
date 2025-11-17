/*
 * Rule: FIO17-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO17-C violation
 */

#include <stdio.h>
#include <stdlib.h>

int main (void) {

    FILE *fp;
    size_t size;
    long length;
    char *buffer;

    fp = fopen("file.txt", "rb");

    if (fp == NULL) {
      /* Handle file open error */
    }

    /* Obtain file size */
    if (fseek(fp, 0, SEEK_END) != 0) {
      /* Handle repositioning error */
    }

    length = ftell(fp);

    if (fseek(fp, 0L, SEEK_SET) != 0) {
      /* Handle repositioning error */
    }

    /* Allocate memory to contain whole file */
    buffer = (char*) malloc(length);
    if (buffer == NULL) {
      /* Handle memory allocation error */
    }
    
    /* size assigned here in some other code */

    if (fread(buffer, 1, size, fp) < size) {
      /* Handle file read error */
    }
    fclose(fp);

    return 0;
}