#include <threads.h>
#include <stdio.h>

typedef struct currency_s {
  int quarters;
  int dimes;
  int nickels;
  int pennies;
} currency_t;
 
currency_t *set_quarters(int quantity, currency_t *currency) {
  currency->quarters += quantity;
  return currency;
}
currency_t *set_dimes(int quantity, currency_t *currency) {
  currency->dimes += quantity;
  return currency;
} 
currency_t *set_nickels(int quantity, currency_t *currency) {
  currency->nickels += quantity;
  return currency;
}
currency_t *set_pennies(int quantity, currency_t *currency) {
  currency->pennies += quantity;
  return currency;
}
 
int init_45_cents(void *currency) {
  currency_t *c = set_quarters(1, set_dimes(2, currency));
  /* Validate values are correct */
  return 0;
}
int init_60_cents(void* currency) {
  currency_t *c = set_quarters(2, set_dimes(1, currency));
  /* Validate values are correct */
  return 0;
}
 
int main(void) {
  thrd_t thrd1;
  thrd_t thrd2;
  currency_t currency = {0, 0, 0, 0};
 
  if (thrd_success != thrd_create(&thrd1, init_45_cents, &currency)) {
    /* Handle error */
  }
  if (thrd_success != thrd_create(&thrd2, init_60_cents, &currency)) {
    /* Handle error */
  }
  if (thrd_success != thrd_join(thrd1, NULL)) {
    /* Handle error */
  }
  if (thrd_success != thrd_join(thrd2, NULL)) {
    /* Handle error */
  }
 
  printf("%d quarters, %d dimes, %d nickels, %d pennies\n",
         currency.quarters, currency.dimes, currency.nickels, currency.pennies);
  return 0;
}