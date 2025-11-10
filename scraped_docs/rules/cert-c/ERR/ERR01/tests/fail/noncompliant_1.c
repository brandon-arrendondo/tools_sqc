errno = 0;
printf("This\n");
printf("is\n");
printf("a\n");
printf("test.\n");
if (errno != 0) {
  fprintf(stderr, "printf failed: %s\n", strerror(errno));
}