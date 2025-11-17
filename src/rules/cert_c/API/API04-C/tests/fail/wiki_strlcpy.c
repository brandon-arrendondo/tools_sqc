/*
 * Rule: API04-C
 * Source: wiki
 * Status: FAIL - Should trigger API04-C violation
 */

char *dir, *file, pname[MAXPATHLEN];

/* ... */

if (strlcpy(pname, dir, sizeof(pname)) >= sizeof(pname)) {
  /* Handle source-string-too long error */
}