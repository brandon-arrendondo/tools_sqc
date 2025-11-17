/*
 * Rule: DCL05-C
 * Source: wiki
 * Status: PASS - Should NOT trigger DCL05-C violation
 */

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