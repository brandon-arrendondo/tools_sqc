/*
 * Rule: ERR07-C
 * Source: wiki
 * Status: FAIL - Should trigger ERR07-C violation
 */

char *file_name;
FILE *fp;

/* Initialize file_name */

fp = fopen(file_name, "r");
if (fp == NULL) {
  /* Handle open error */
}

/* Read data */

rewind(fp);

/* Continue */