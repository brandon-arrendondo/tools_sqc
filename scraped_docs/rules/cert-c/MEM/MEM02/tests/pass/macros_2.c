widget *p;

/* ... */

p = MALLOC(widget);   /* OK */
if (p != NULL) {
  p->i = 0;           /* OK */
  p->d = 0.0;         /* OK */
}