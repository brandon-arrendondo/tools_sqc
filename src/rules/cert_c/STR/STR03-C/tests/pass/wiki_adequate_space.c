/*
 * Rule: STR03-C
 * Source: wiki
 * Status: PASS - Should NOT trigger STR03-C violation
 */

char *string_data = NULL;
char a[16];

/* ... */

if (string_data == NULL) {
  /* Handle null pointer error */
}
else if (strlen(string_data) >= sizeof(a)) {
  /* Handle overlong string error */
}
else {
  strcpy(a, string_data);
}