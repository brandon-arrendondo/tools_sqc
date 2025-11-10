#include <stdlib.h>
#include <limits.h>
 
void *AllocBlocks(size_t cBlocks) {
  if (cBlocks == 0) { return NULL; }
  unsigned long long alloc = cBlocks * 16;
  return (alloc < UINT_MAX) ? malloc(cBlocks * 16) : NULL;
}