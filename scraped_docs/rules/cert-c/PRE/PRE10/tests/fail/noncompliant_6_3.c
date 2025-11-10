if (x > y) { /* Single-branch if-statement!!! */

  tmp = x;   /* The one and only branch consists */
  x = y;     /* of the block. */
  y = tmp;
}
;            /* Empty statement */
else         /* ERROR!!! "parse error before else" */
  do_something();