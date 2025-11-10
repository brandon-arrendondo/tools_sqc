errno_t retValue; 
string_m dest, source;  

/* ... */

if (retValue = strcpy_m(dest, source)) { 
  fprintf(stderr, "Error %d from strcpy_m.\n", retValue);
}