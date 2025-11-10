#include <Windows.h>
/*
  typedef struct tagPOINT {
    long x, y;
  } POINT, *LPPOINT;
*/
 
void func(const LPPOINT pt) {
  /* Can modify pt's contents, against expectations */
}