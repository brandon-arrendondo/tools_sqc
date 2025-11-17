/*
 * Rule: INT16-C
 * Source: wiki
 * Status: FAIL - Should trigger INT16-C violation
 */

int value;

if (scanf("%d", &value) == 1) {
  if (value & 0x1 != 0) {
    /* Take action if value is odd */
  }
}