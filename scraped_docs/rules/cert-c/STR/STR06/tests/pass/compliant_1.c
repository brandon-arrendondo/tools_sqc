char *token;
const char *path = getenv("PATH");
/* PATH is something like "/usr/bin:/bin:/usr/sbin:/sbin" */

char *copy = (char *)malloc(strlen(path) + 1);
if (copy == NULL) {
  /* Handle error */
}
strcpy(copy, path);
token = strtok(copy, ":");
puts(token);

while (token = strtok(0, ":")) {
  puts(token);
}

free(copy);
copy = NULL;

printf("PATH: %s\n", path);
/* PATH is still "/usr/bin:/bin:/usr/sbin:/sbin" */