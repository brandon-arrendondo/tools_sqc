struct myData {
  char c;
  long l;
};

/* ... */

FILE *file;
struct myData data;
char buf[25];
char *end_ptr;

/* Initialize file */

if (fgets(buf, 1, file) == NULL) {
  /* Handle error */
}

data.c = buf[0];

if (fgets(buf, sizeof(buf), file) == NULL) {
  /* Handle Error */
}

data.l = strtol(buf, &end_ptr, 10);

if ((ERANGE == errno)
 || (end_ptr == buf)
 || ('\n' != *end_ptr && '\0' != *end_ptr)) {
    /* Handle Error */
}