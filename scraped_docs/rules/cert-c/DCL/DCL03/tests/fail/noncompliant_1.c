#include <assert.h>
 
struct timer {
  unsigned char MODE;
  unsigned int DATA;
  unsigned int COUNT;
};
 
int func(void) {
  assert(sizeof(struct timer) == sizeof(unsigned char) + sizeof(unsigned int) + sizeof(unsigned int));
}