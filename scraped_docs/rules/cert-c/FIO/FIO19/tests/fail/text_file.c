FILE *fp;
long file_size;
char *buffer;

fp = fopen("foo.txt", "r");
if (fp == NULL) {
  /* Handle error */
}

if (fseek(fp, 0 , SEEK_END) != 0) {
  /* Handle error */
}

file_size = ftell(fp);
if (file_size == -1) {
  /* Handle error */
}

buffer = (char*)malloc(file_size);
if (buffer == NULL) {
  /* Handle error */
}

/* ... */