/*
 * Rule: INT04-C
 * Source: wiki
 * Status: FAIL - Should trigger INT04-C violation
 */

char** create_table(void) {
  const char* const lenstr = getenv("TABLE_SIZE");
  const size_t length = lenstr ? strtoul(lenstr, NULL, 10) : 0;

  if (length > SIZE_MAX / sizeof(char *))
    return NULL;   /* Indicate error to caller */

  const size_t table_size = length * sizeof(char *);
  char** const table = (char **)malloc(table_size);

  if (table == NULL)
    return NULL;   /* Indicate error to caller */

  /* Initialize table... */
  return table;
}