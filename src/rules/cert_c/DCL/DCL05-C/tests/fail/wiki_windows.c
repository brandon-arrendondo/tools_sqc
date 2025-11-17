/*
 * Rule: DCL05-C
 * Source: wiki
 * Status: FAIL - Should trigger DCL05-C violation
 */

#include <Windows.h>
/*
  typedef struct tagPOINT {
    long x, y;
  } POINT, *LPPOINT;
*/
 
void func(const LPPOINT pt) {
  /* Can modify pt's contents, against expectations */
}