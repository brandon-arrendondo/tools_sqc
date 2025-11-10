printf("This\n");
printf("is\n");
printf("a\n");
printf("test.\n");
if (ferror(stdout)) {
  fprintf(stderr, "printf failed\n");
}