/*
 * Rule: FIO15-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO15-C violation
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

int secure_dir(const char *path) {
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

void safe_directory_operation(void) {
  char *dir_name = "/tmp/secure";
  const char *file_name = "passwd";
  
  // OK: Checking directory is secure before using it
  if (!secure_dir(dir_name)) {
    return;
  }
  
  if (chdir(dir_name) == -1) {
    return;
  }
  
  FILE *fp = fopen(file_name, "w");
  if (fp == NULL) {
    return;
  }
  
  fprintf(fp, "data");
  
  if (fclose(fp) != 0) {
    return;
  }
  
  if (remove(file_name) != 0) {
    return;
  }
}