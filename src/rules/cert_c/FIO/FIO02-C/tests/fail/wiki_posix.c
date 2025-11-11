/*
 * Rule: FIO02-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO02-C violation
 */

char *realpath_res = NULL;
char *canonical_filename = NULL;
size_t path_size = 0;
long pc_result;

/* Verify argv[1] is supplied */

errno = 0;

/* Query for PATH_MAX */
pc_result = pathconf(argv[1], _PC_PATH_MAX);

if ( (pc_result == -1) && (errno != 0) ) {
  /* Handle error */
} else if (pc_result == -1) {
  /* Handle error */
} else if (pc_result <= 0) {
  /* Handle error */
}
path_size = (size_t)pc_result;

if (path_size > 0) {
  canonical_filename = malloc(path_size);

  if (canonical_filename == NULL) {
    /* Handle error */
  }

  realpath_res = realpath(argv[1], canonical_filename);
}

if (realpath_res == NULL) {
  /* Handle error */
}

if (!verify_file(realpath_res)) {
  /* Handle error */
}

if (fopen(realpath_res, "w") == NULL) {
  /* Handle error */
}

/* ... */

free(canonical_filename);
canonical_filename = NULL;