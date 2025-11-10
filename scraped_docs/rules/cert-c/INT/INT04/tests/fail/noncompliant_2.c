const char *table[] = { "black", "white", "blue", "green" };
 
const char *set_background_color(void) {
  int color_index;
  GET_TAINTED_INTEGER(int, color_index);
 
  const char *color = table[color_index];  /* Violation */
 
  /* ... */
  return color;
}