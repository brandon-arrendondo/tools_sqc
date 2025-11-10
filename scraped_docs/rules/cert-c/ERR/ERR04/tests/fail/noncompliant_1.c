#include <stdlib.h>
#include <stdio.h>

int write_data(void) {
  const char *filename = "hello.txt";
  FILE *f = fopen(filename, "w");
  if (f == NULL) {
    /* Handle error */
  }
  fprintf(f, "Hello, World\n");
  /* ... */
  abort(); /* Oops! Data might not be written! */
  /* ... */
  return 0;
}

int main(void) {
  write_data();
  return EXIT_SUCCESS;
}