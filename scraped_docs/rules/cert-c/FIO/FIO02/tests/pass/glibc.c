/* Verify argv[1] is supplied */

char *canonical_filename = canonicalize_file_name(argv[1]);
if (canonical_filename == NULL) {
  /* Handle error */
}

/* Verify file name */

if (fopen(canonical_filename, "w") == NULL) {
  /* Handle error */
}

/* ... */

free(canonical_filename);
canonical_filename = NULL;