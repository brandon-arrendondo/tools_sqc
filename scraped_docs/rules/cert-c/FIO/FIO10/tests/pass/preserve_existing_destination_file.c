const char *src_file = /* ... */;
const char *dest_file = /* ... */;

if (!file_exists(dest_file)) {
  if (rename(src_file, dest_file) != 0) {
    /* Handle error condition */
  }
} 
else {
  /* Handle file-exists condition */
}