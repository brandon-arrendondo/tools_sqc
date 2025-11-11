/*
 * Rule: WIN00-C
 * Source: wiki
 * Status: PASS - Should NOT trigger WIN00-C violation
 */

#include <Windows.h>
 
void func(void) {
  HMODULE hMod = LoadLibraryEx(TEXT("MyLibrary.dll"), NULL,
                               LOAD_LIBRARY_SEARCH_APPLICATION_DIR |
                               LOAD_LIBRARY_SEARCH_SYSTEM32);
  if (hMod != NULL) {
    typedef void (__cdecl func_type)(void);
    func_type *fn = (func_type *)GetProcAddress(hMod, "MyFunction");
    if (fn != NULL)
      fn();
  }
}