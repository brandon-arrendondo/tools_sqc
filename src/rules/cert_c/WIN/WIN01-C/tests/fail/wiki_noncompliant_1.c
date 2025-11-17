/*
 * Rule: WIN01-C
 * Source: wiki
 * Status: FAIL - Should trigger WIN01-C violation
 */

#include <Windows.h>
 
DWORD ThreadID;  /* Filled in by call to CreateThread */
 
/* Thread 1 */
DWORD WINAPI ThreadProc(LPVOID param) {
  /* Performing work */
}
 
/* Thread 2 */
HANDLE hThread = OpenThread(THREAD_TERMINATE, FALSE, ThreadID);
if (hThread) {
  TerminateThread(hThread, 0xFF);
  CloseHandle(hThread);
}