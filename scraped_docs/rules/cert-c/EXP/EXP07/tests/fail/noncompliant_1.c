#include <stdio.h>
/* ... */
nblocks = 1 + ((nbytes - 1) >> 9); /* BUFSIZ = 512 = 2^9 */