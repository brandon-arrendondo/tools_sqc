/*
 * Rule: FIO42-C
 * Source: wiki
 * Status: FAIL - Should trigger FIO42-C violation
 */

#include <Windows.h>

int func(LPCTSTR filename) {
  HANDLE hFile = CreateFile(filename, GENERIC_READ, 0, NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL, NULL);
  if (INVALID_HANDLE_VALUE == hFile) {
    return -1;
  }
  /* ... */
  return 0;
}