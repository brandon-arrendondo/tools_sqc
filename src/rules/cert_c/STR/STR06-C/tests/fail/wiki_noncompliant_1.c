/*
 * Rule: STR06-C
 * Source: wiki
 * Status: FAIL - Should trigger STR06-C violation
 */

char *token;
char *path = getenv("PATH");

token = strtok(path, ":");
puts(token);

while (token = strtok(0, ":")) {
  puts(token);
}

printf("PATH: %s\n", path);
/* PATH is now just "/usr/bin" */