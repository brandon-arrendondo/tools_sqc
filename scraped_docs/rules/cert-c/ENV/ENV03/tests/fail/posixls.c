if (system("/bin/ls dir.`date +%Y%m%d`") == -1) {
  /* Handle error */
}