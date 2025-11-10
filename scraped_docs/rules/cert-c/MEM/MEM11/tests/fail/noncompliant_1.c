#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum {MAX_LENGTH=100};

typedef struct namelist_s {
  char name[MAX_LENGTH];
  struct namelist_s* next;
} *namelist_t;

int main() {
  namelist_t names = NULL;
  char new_name[MAX_LENGTH];

  do {
    /* 
     * Adding unknown number of records to a list;
     * the user can enter as much data as he wants
     * and exhaust the heap.
     */
    puts("To quit, enter \"quit\"");
    puts("Enter record:");
    fgets(new_name, MAX_LENGTH, stdin);
    if (strcmp(new_name, "quit") != 0) {
      /* 
       * Names continue to be added without bothering 
       * about the size on the heap.
       */
      unsigned int i = strlen(new_name) - 1;
      if (new_name[i] == '\n') new_name[i] = '\0';
      namelist_t new_entry = (namelist_t) malloc(sizeof( struct namelist_s));
      if (new_entry == NULL) {
        /* Handle error */
      }
      strcpy( new_entry->name, new_name);
      new_entry->next = names;
      names = new_entry;
    }
    puts(new_name);
  } while (strcmp( new_name, "quit") != 0);

  return 0;
}