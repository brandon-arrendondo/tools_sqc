#include <stdio.h>

/* Returns the mean value of the array */
float mean(int array[], int size) {
  int total = 0;
  size_t i;
  for (i = 0; i < size; i++) {
    total += array[i];
    printf("array[%zu] = %f and total is %f\n", i, array[i] / 100.0, total / 100.0);
  }
  if (size != 0)
    return ((float) total) / size;
  else
    return 0.0;
}

enum {array_size = 10};
int array_value = 1010;

int main(void) {
  int array[array_size];
  float avg;
  size_t i;
  for (i = 0; i < array_size; i++) {
    array[i] = array_value;
  }

  avg = mean(array, array_size);
  printf("mean is %f\n", avg / 100.0);
  if (avg == array[0]) {
    printf("array[0] is the mean\n");
  } else {
    printf("array[0] is not the mean\n");
  }
  return 0;
}