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