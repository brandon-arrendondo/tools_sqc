#include <stdio.h>

int main(void) {
  #if defined(__STDC__)
    #if (__STDC__ == 1)
      printf("Implementation is ISO-conforming.\n");
    #else
      printf("Implementation is not ISO-conforming.\n");
    #endif
  #else   /* !defined(__STDC__) */
    printf("__STDC__ is not defined.\n");
  #endif
  /* ... */
  return 0;
}