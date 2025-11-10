int a = 2;
int b = 2;
int c = 2;
/* ... */
if (a < b < c) /* Misleading; likely bug */
/* ... */
if (a == b == c) /* Misleading; likely bug */