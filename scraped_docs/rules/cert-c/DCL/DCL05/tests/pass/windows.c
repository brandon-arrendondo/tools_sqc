#include <Windows.h>
/*
  typedef struct tagPOINT {
    long x, y;
  } POINT, *LPPOINT;
*/
 
typedef const POINT *LPCPOINT;
void func(LPCPOINT pt) {
  /* Cannot modify pt's contents */
}