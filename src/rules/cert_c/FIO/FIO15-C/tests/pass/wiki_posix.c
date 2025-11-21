/*
 * Rule: FIO15-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO15-C violation
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int is_secure_directory(const char *path) {
  struct stat buf;
  uid_t my_uid = geteuid();

  if (lstat(path, &buf) != 0) {
    return 0;
  }

  if (!S_ISDIR(buf.st_mode)) {
    return 0;
  }

  if ((buf.st_uid != my_uid) && (buf.st_uid != 0)) {
    return 0;
  }

  if (buf.st_mode & (S_IWGRP | S_IWOTH)) {
    return 0;
  }

  return 1;
}

void safe_file_operation(void) {
  const char *path = "/tmp/myfile";

  // OK: Checking if directory is secure before performing file operations
  if (is_secure_directory("/tmp")) {
    FILE *fp = fopen(path, "w");
    if (fp) {
      fprintf(fp, "data");
      fclose(fp);
    }
  }
}