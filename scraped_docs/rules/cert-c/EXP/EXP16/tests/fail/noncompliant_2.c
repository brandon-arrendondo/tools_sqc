/* First the options that are allowed only for root */
if (getuid() == 0 || geteuid != 0) {
  /* ... */
}