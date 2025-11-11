/*
 * Rule: FIO02-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO02-C violation
 */

/* Verify argv[1] is supplied */

if (!verify_file(argv[1])) {
  /* Handle error */
}

if (fopen(argv[1], "w") == NULL) {
  /* Handle error */
}

/* ... */