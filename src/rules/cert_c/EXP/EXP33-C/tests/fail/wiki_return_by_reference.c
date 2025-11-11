/*
 * Rule: EXP33-C
 * Source: wiki
 * Status: FAIL - Should trigger EXP33-C violation
 */

void set_flag(int number, int *sign_flag) {
  if (NULL == sign_flag) {
    return;
  }

  if (number > 0) {
    *sign_flag = 1;
  } else if (number < 0) {
    *sign_flag = -1;
  }
}

int is_negative(int number) {
  int sign;
  set_flag(number, &sign);
  return sign < 0;
}