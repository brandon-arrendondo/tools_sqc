/*
 * Rule: WIN03-C
 * Source: wiki
 * Status: FAIL - Should trigger WIN03-C violation
 */

#include <Windows.h>
 
void func(void) {
  HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, TRUE, TEXT("Global\\CommonMutex"));
  if (!hMutex) {
    /* Handle error */
  }
}