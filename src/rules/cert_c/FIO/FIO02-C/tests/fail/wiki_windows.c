/*
 * Rule: FIO02-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO02-C violation
 */

/* ... */

enum { INITBUFSIZE = 256 };
DWORD ret = 0;
DWORD new_ret = 0;
char *canonical_filename;
char *new_file;
char *file_name;

/* ... */

file_name = (char *)malloc(strlen(argv[1])+1);
canonical_filename = (char *)malloc(INITBUFSIZE);

if ( (file_name != NULL) && (canonical_filename != NULL) ) {
  strcpy(file_name, argv[1]);
  strcpy(canonical_filename, "");
} else {
  /* Handle error */
}

ret = GetFullPathName(
  file_name,
  INITBUFSIZE,
  canonical_filename,
  NULL
);

if (ret == 0) {
  /* Handle error */
}
else if (ret > INITBUFSIZE) {
  new_file = (char *)realloc(canonical_filename, ret);
  if (new_file == NULL) {
    /* Handle error */
  }

  canonical_filename = new_file;

  new_ret = GetFullPathName(
    file_name,
    ret,
    canonical_filename,
    NULL
  );
  if (new_ret > ret) {
    /*
     * The length of the path changed between calls
     * to GetFullPathName(); handle error.
     */
  }
  else if (new_ret == 0) {
    /* Handle error */
  }
}

if (!verify_file(canonical_filename)) {
  /* Handle error */
}
/* Verify file name before using */