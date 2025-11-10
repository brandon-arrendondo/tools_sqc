float currentBalance; /* User's cash balance */

void doDeposit() {
  float val;

  scanf("%f", &val);
  if (isinf(val)) {
    /* Handle infinity error */
  }
  if (isnan(val)) {
    /* Handle NaN error */
  }
  if (val >= MAX_VALUE - currentBalance) {
    /* Handle range error */
  }

  currentBalance += val;
}