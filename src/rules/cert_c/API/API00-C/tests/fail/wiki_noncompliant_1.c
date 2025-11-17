/*
 * Rule: API00-C
 * Source: wiki
 * Status: FAIL - Should trigger API00-C violation
 */

/* Sets some internal state in the library */
extern int setfile(FILE *file);

/* Performs some action using the file passed earlier */
extern int usefile();

static FILE *myFile;

void setfile(FILE *file) {
    myFile = file;
}

void usefile(void) {
    /* Perform some action here */
}