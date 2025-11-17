/*
 * Rule: FIO09-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO09-C violation
 */

struct myData {
  char c;
  long l;
};

/* ... */

FILE *file;
struct myData data;

/* Initialize file */

if (fread(&data, sizeof(struct myData), 1, file) < sizeof(struct myData)) {
  /* Handle error */
}