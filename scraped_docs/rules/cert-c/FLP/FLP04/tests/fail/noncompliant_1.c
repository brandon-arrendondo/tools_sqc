float currentBalance; /* User's cash balance */
void doDeposit() {
  float val;

  scanf("%f", &val);

  if(val >= MAX_VALUE - currentBalance) {
    /* Handle range error */
  }

  currentBalance += val;
}