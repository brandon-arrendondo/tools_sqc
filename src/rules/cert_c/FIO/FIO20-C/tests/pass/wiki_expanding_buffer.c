/*
 * Rule: FIO20-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO20-C violation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_filled_buffer(void) {
  char temp[32];
  char *ret = NULL;
  size_t full_length = 0;
   
  while (fgets(temp, sizeof(temp), stdin)) {
    size_t len = strlen(temp);
    if (SIZE_MAX - len - 1 < full_length) {
      break;
    }
    char *r_temp = realloc(ret, full_length + len + 1);
    if (r_temp == NULL) {
      break;
    }
    ret = r_temp;
    strcpy(ret + full_length, temp); /* concatenate */
    full_length += len;
   
    if (feof(stdin) || temp[len-1] == '\n') {
      return ret;
    }
  }

  free(ret);
  return NULL;
}