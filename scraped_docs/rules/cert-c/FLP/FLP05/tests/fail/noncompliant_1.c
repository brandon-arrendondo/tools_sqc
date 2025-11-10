#include <stdio.h>
float x = 1/3.0;
printf("Original    : %e\n", x);
x = x * 7e-45;
printf("Denormalized: %e\n", x);
x = x / 7e-45;
printf("Restored    : %e\n", x);