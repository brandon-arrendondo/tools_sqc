/*
 * Rule: FIO22-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO22-C violation
 */

#include <stdio.h>
#include <stdlib.h>
 
extern const char *get_validated_editor(void);
 
void func(const char *file_name) {
  char *editor;
  int fd = open(file_name, O_RDONLY | O_CLOEXEC);
  if (fd == -1) {
    /* Handle error */
  }
 
  editor = get_validated_editor();
  if (editor == NULL) {
    /* Handle error */
  }
 
  if (system(editor) == -1) {
    /* Handle error */
  }
}