const char *src_file = /* ... */;
const char *dest_file = /* ... */;

(void)remove(dest_file);

if (rename(src_file, dest_file) != 0) {
  /* Handle error condition */
}