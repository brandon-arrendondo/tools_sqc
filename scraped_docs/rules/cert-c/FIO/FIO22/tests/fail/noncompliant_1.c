#include <stdio.h>
#include <stdlib.h>
 
extern const char *get_validated_editor(void);
 
void func(const char *file_name) {
  FILE *f;
  const char *editor;

  f = fopen(file_name, "r");
  if (f == NULL) {
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