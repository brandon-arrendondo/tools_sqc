#include <Windows.h>
 
DWORD ThreadID;  /* Filled in by call to CreateThread */
LONG ShouldThreadExit = 0;

/* Thread 1 */
DWORD WINAPI ThreadProc(LPVOID param) {
  while (1) {
    /* Performing work */
    if (1 == InterlockedCompareExchange(&ShouldThreadExit, 0, 1))
      return 0xFF;
  }
}
 
/* Thread 2 */
InterlockedExchange(&ShouldThreadExit, 1);