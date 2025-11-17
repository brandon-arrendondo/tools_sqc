/*
 * Rule: POS54-C
 * Source: wiki
 * Status: FAIL - Should trigger POS54-C violation
 */

#include <stdio.h>
#include <string.h>
 
int main(int argc, char *argv[]) {
  FILE *out;
  FILE *in;
  size_t size;
  char *ptr;
 
  if (argc != 2) {
    /* Handle error */
  }
 
  in = fmemopen(argv[1], strlen(argv[1]), "r");
  /* Use in */
 
  out = open_memstream(&ptr, &size);
  /* Use out */
 
  return 0; 
}