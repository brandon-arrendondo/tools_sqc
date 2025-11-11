/*
 * Rule: WIN00-C
 * Source: wiki
 * Status: FAIL - Should trigger WIN00-C violation
 */

#include <Windows.h>
 
void func(void) {
  HMODULE hMod = LoadLibrary(TEXT("MyLibrary.dll"));
  if (hMod != NULL) {
    typedef void (__cdecl func_type)(void);
    func_type *fn = (func_type *)GetProcAddress(hMod, "MyFunction");
    if (fn != NULL)
      fn();
  }
}