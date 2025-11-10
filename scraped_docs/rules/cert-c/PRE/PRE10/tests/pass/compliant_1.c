/*
 * Swaps two values and requires
 * tmp variable to be defined.
 */
#define SWAP(x, y) \
  do { \
    tmp = (x); \
    (x) = (y); \
    (y) = tmp; } \
  while (0)