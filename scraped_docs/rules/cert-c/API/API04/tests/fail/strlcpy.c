char *dir, *file, pname[MAXPATHLEN];

/* ... */

if (strlcpy(pname, dir, sizeof(pname)) >= sizeof(pname)) {
  /* Handle source-string-too long error */
}