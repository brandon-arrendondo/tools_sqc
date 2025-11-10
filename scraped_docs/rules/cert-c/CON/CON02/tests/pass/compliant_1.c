#include <threads.h>

int account_balance;
mtx_t flag;

/* Initialize flag */

int debit(unsigned int amount) {
  if (mtx_lock(&flag) == thrd_error) {
    return -1;  /* Indicate error */
  }
 
  account_balance -= amount; /* Inside critical section */

  if (mtx_unlock(&flag) == thrd_error) {
    return -1;  /* Indicate error */
  }

  return 0;
}