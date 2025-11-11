/*
 * Rule: INT10-C
 * Source: wiki
 * Status: PASS - Should NOT trigger INT10-C violation
 */

int insert(size_t* result, size_t index, int *list, size_t size, int value) {
  if (size != 0 && size != SIZE_MAX) {
    index = (index + 1) % size;
    list[index] = value;
    *result = index;
    return 1;
  }
  else {
    return 0;
  }
}