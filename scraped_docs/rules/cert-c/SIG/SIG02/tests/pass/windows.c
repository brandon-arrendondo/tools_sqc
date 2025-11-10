#include <windows.h>

/* 
 * Note that the CRITICAL_SECTION must be initialized with
 * InitializeCriticalSection, and the CONDITION_VARIABLE must
 * be initialized with InitializeConditionVariable prior to 
 * using them.
 */
CRITICAL_SECTION CritSection;
CONDITION_VARIABLE ConditionVar;

/* THREAD 1 */
int do_work(void) {
  /* ... */
  WakeConditionVariable(&ConditionVar);
}

/* THREAD 2 */
int wait_and_work(void) {

  EnterCriticalSection(&CritSection);
  SleepConditionVariableCS(&ConditionVar, &CritSection, INFINITE);
  LeaveCriticalSection(&CritSection);
  /* ... */
}