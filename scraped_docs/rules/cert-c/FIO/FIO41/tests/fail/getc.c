#include <stdio.h>
 
void func(const char *file_name) {
  FILE *fptr;

  int c = getc(fptr = fopen(file_name, "r"));
  if (feof(fptr) || ferror(fptr)) {
    /* Handle error */
  }

  if (fclose(fptr) == EOF) {
    /* Handle error */
  }
}