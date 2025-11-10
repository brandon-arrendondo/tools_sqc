#include <stdlib.h>
#include <assert.h>
#include <limits.h>
 
static_assert(
  CHAR_BIT * sizeof(unsigned long long) >= 
  CHAR_BIT * sizeof(size_t) + 4, 
  "Unable to detect wrapping after multiplication"
);

void *AllocBlocks(size_t cBlocks) {
  if (cBlocks == 0) return NULL;
  unsigned long long alloc = (unsigned long long)cBlocks * 16;
  return (alloc < UINT_MAX) ? malloc(cBlocks * 16) : NULL;
}