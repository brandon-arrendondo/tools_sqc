/*
 * Rule: FIO03-C
 * Source: wiki
 * Status: PASS - Should NOT trigger FIO03-C violation
 */

TCHAR *file_name;
HANDLE hFile = CreateFile(file_name, GENERIC_READ | GENERIC_WRITE, 0, 0, 
                          CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
if (INVALID_HANDLE_VALUE == hFile) {
  DWORD err = GetLastError();
  if (ERROR_FILE_EXISTS == err) {
    /* Handle file exists error */
  } else {
    /* Handle other error */
  }
}