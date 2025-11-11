/*
 * Rule: FIO15-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO15-C violation
 */

#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

enum { MAX_SYMLINKS = 5 };

/* Returns nonzero if directory is secure, zero otherwise */
int secure_dir(const char *fullpath) {
  static unsigned int num_symlinks = 0;
  char *path_copy = NULL;
  char **dirs = NULL;
  int num_of_dirs = 1;
  int secure = 1;
  int i, r;
  struct stat buf;
  uid_t my_uid = geteuid();
  size_t linksize;
  char* link;
   
  if (!fullpath || fullpath[0] != '/') {
    /* Handle error */
  }
   
  if (num_symlinks > MAX_SYMLINKS) {  /* Could be a symlink loop */
    /* Handle error */
  }
  
  if (!(path_copy = strdup(fullpath))) {
    /* Handle error */
  }
  
  /* Figure out how far it is to the root */
  char* path_parent = path_copy;
  for (; ((strcmp(path_parent, "/") != 0) &&
          (strcmp(path_parent, "//") != 0) &&
          (strcmp(path_parent, ".") != 0));
       path_parent = dirname(path_parent)) {
    num_of_dirs++;
  } /* Now num_of_dirs indicates # of dirs we must check */
  free(path_copy);
  path_copy = NULL;
  
  if (!(dirs = (char **)malloc(num_of_dirs * sizeof(char *)))) {
    /* Handle error */
  }
   
  if (!(dirs[num_of_dirs - 1] = strdup(fullpath))) {
    /* Handle error */
  }
  
  if (!(path_copy = strdup(fullpath))) {
    /* Handle error */
  }
  
  /* Now fill the dirs array */
  path_parent = path_copy;
  for (i = num_of_dirs - 2; i >= 0; i--) {
    path_parent = dirname(path_parent);
    if (!(dirs[i] = strdup(path_parent))) {
      /* Handle error */
    }
  }
  free(path_copy);
  path_copy = NULL;
  
  /*
   * Traverse from the root to the fullpath,
   * checking permissions along the way.
   */
 for (i = 0; i < num_of_dirs; i++) {
    if (lstat(dirs[i], &buf) != 0) {
      /* Handle error */
    }
     
    if (S_ISLNK(buf.st_mode)) { /* Symlink, test linked-to file */
      linksize = buf.st_size + 1;
      if (!(link = (char *)malloc(linksize))) {
        /* Handle error */
      }
       
      r = readlink(dirs[i], link, linksize);
      if (r == -1) {
        /* Handle error */
      } else if (r >= linksize) {
        /* Handle truncation error */
      }
      link[r] = '\0';
 
      num_symlinks++;
      r = secure_dir(link);
      num_symlinks--;
       
      if (!r) {
        secure = 0;
 
        free(link);
        link = NULL;
        break;
      }
       
      free(link);
      link = NULL;
       
      continue;
    }
     
    if (!S_ISDIR(buf.st_mode)) { /* Not a directory */
      secure = 0;
      break;
    }
     
    if ((buf.st_uid != my_uid) && (buf.st_uid != 0)) {
      /* Directory is owned by someone besides user or root */
      secure = 0;
      break;
    }
     
    if (buf.st_mode & (S_IWGRP | S_IWOTH)) { /* dir is writable by others */
      secure = 0;
      break;
    }
  }
   
  for (i = 0; i < num_of_dirs; i++) {
    free(dirs[i]);
    dirs[i] = NULL;
  }
  
  free(dirs);
  dirs = NULL;
  
  return secure;
}