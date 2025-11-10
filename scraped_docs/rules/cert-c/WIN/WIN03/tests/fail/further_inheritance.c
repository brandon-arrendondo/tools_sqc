#include <Windows.h>
 
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR cmdLine, int show) {
  HANDLE hFile = (HANDLE)_strtoui64(cmdLine, NULL, 16);
 
  /* Continue working with the file */
}