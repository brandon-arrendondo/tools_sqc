/* Sets some internal state in the library */
extern errno_t setfile(FILE *file);

/* Performs some action using the file passed earlier */
extern errno_t usefile(void);

static FILE *myFile;

errno_t setfile(FILE *file) {
 if (file && !ferror(file) && !feof(file)) {
    myFile = file;
    return 0;
  }

  /* Error safety: leave myFile unchanged */
  return -1;
}

errno_t usefile(void) {
  if (!myFile) return -1;

    /*
     * Perform other checks if needed; return 
     * error condition.
     */

    /* Perform some action here */
    return 0;
}