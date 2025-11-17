/*
 * Rule: WIN03-C
 * Source: wiki
 * Status: PASS - Should NOT trigger WIN03-C violation
 */

#include <Windows.h>
 
void func(void) {
  HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, TEXT("Global\\CommonMutex"));
  if (!hMutex) {
    /* Handle error */
  }
}