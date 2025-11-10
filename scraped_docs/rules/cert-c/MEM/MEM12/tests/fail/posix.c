typedef struct object {  /* Generic struct: contents don't matter */
  int propertyA, propertyB, propertyC;
} object_t;

errno_t do_something(void){
  FILE *fin1, *fin2;
  object_t *obj;
  errno_t ret_val;
  
  fin1 = fopen("some_file", "r");
  if (fin1 == NULL) {
    return errno;
  }

  fin2 = fopen("some_other_file", "r");
  if (fin2 == NULL) {
    fclose(fin1);
    return errno;
  }

  obj = malloc(sizeof(object_t));
  if (obj == NULL) {
    ret_val = errno;
    fclose(fin1);
    return ret_val;  /* Forgot to close fin2!! */
  }

  /* ... More code ... */

  fclose(fin1);
  fclose(fin2);
  free(obj);
  return NOERR;
}