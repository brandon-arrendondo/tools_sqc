FILE *fp;
fpos_t pos;
char *file_name;

/* Initialize file_name */

fp = fopen(file_name, "rb");
if (fp == NULL) {
  /* Handle error */
}

/* Read data */

if (fgetpos(fp, &pos)) {
  /* Handle error */
}

/* Read the data that will be "pushed back" */

if (fsetpos(fp, &pos)) {
  /* Handle error */
}

/* Continue */