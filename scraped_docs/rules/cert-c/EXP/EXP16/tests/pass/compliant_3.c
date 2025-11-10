int do_xyz(void); 
 
int f(void) {
/* ... */ 
  if (do_xyz()) { 
    return -1; /* Indicate failure */
  }
/* ... */
  return 0;  
}