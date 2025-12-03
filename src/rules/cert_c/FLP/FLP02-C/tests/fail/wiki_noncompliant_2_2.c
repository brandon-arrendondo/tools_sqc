/*
 * Rule: FLP02-C
 * Source: wiki
 * Status: FAIL - Should trigger FLP02-C violation
 */

#include <stdio.h>

/* Returns the mean value of the array */
float mean(float array[], int size) {
  float total = 0.0;
  size_t i;
  for (i = 0; i < size; i++) {
    total += array[i];
    printf("array[%zu] = %f and total is %f\n", i, array[i], total);
  }
  if (size != 0)
    return total / size;
  else
    return 0.0;
}

enum { array_size = 10 };
float array_value = 10.1;

int main(void) {
  float array[array_size];
  float avg;
  size_t i;
  for (i = 0; i < array_size; i++) {
    array[i] = array_value;
  }

  avg = mean( array, array_size);
  printf("mean is %f\n", avg);
  if (avg == array[0]) {  /* FLP02-C violation: float equality comparison */
    printf("array[0] is the mean\n");
  } else {
    printf("array[0] is not the mean\n");
  }
  return 0;
}
