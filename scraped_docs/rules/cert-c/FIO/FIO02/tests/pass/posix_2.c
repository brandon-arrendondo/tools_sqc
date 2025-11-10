char *realpath_res = NULL;

/* Verify argv[1] is supplied */

realpath_res = realpath(argv[1], NULL);
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

free(realpath_res);
realpath_res = NULL;