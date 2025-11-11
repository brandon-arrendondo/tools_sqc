/*
 * Rule: ERR07-C
 * Source: wiki
 * Status: PASS - Should NOT trigger ERR07-C violation
 */

char *file_name;
FILE *fp;

/* Initialize file_name */

fp = fopen(file_name, "r");
if (fp == NULL) {
  /* Handle open error */
}

/* Read data */

if (fseek(fp, 0L, SEEK_SET) != 0) {
  /* Handle repositioning error */
}

/* Continue */