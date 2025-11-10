#include <stdio.h>
 
void func(const char *file_name) {
  FILE *fp = fopen(file_name, "wb+");
  if (fp == NULL) {
    /* Handle error */
  }
}