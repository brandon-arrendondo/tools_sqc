#include <Windows.h>
 
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR cmdLine, int show) {
  HANDLE hUntrusted = (HANDLE)_strtoui64(cmdLine, NULL, 16);
  HANDLE hFile = NULL;
  BY_HANDLE_FILE_INFORMATION info;
 
  if (!DuplicateHandle(GetCurrentProcess(), hUntrusted, GetCurrentProcess(), &hFile,
                       0, FALSE, DUPLICATE_SAME_ACCESS | DUPLICATE_CLOSE_SOURCE)) {
    /* Handle error; possibly not even a valid handle */
  }
  
  if (!GetFileInformationByHandle(hFile, &info)) {
    /* Handle error; likely not a valid file handle */
 
    // Close the file handle since we no longer trust it.
    CloseHandle(hFile);
    hFile = NULL;
  }
  
  /* Continue working with the file */
}