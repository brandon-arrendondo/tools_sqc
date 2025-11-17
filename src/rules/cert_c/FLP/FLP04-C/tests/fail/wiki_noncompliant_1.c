/*
 * Rule: FLP04-C
 * Source: wiki
 * Status: FAIL - Should trigger FLP04-C violation
 */

float currentBalance; /* User's cash balance */
void doDeposit() {
  float val;

  scanf("%f", &val);

  if(val >= MAX_VALUE - currentBalance) {
    /* Handle range error */
  }

  currentBalance += val;
}