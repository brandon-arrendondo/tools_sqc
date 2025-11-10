char *token;
char *path = getenv("PATH");

token = strtok(path, ":");
puts(token);

while (token = strtok(0, ":")) {
  puts(token);
}

printf("PATH: %s\n", path);
/* PATH is now just "/usr/bin" */