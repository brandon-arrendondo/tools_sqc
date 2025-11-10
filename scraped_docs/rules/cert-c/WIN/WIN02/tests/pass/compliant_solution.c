#include <Windows.h>
#include <sddl.h>
 
static void launch_notepad_as_user(HANDLE token) {
  PROCESS_INFORMATION pi;
  STARTUPINFO si;
 
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof( si );
  if (CreateProcessAsUser(token, TEXT("C:\\Windows\\Notepad.exe"), NULL, NULL,
                          NULL, FALSE, 0, NULL, NULL, &si, &pi )) {
    /* Process has been created; work with the process and wait for it to
       terminate. */
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
  }
}
 
static BOOL adjust_token_integrity_level(HANDLE token, const char *sid) {
  /* Convert the string SID to a SID *, then adjust the token's
     privileges. */
  BOOL ret;
  PSID psd = NULL;
  if (ConvertStringSidToSidA(sid, &psd)) {
    TOKEN_MANDATORY_LABEL tml;
    
    ZeroMemory(&tml, sizeof(tml));
    tml.Label.Attributes = SE_GROUP_INTEGRITY;
    tml.Label.Sid = psd;
 
    ret = SetTokenInformation(token, TokenIntegrityLevel, &tml,
                              sizeof(tml) + GetLengthSid(psd));
    
    LocalFree(psd);
  }
  return ret;
}
 
void launch_notepad(void) {
  /* Low level; see table for integrity level string names */
  const char *requested_sid = "S-1-16-4096";
  HANDLE token_cur, token_dup;
  /* Get the current process' security token as a starting point, then modify
     a duplicate so that it runs with a fixed integrity level. */
  if (OpenProcessToken(GetCurrentProcess(), TOKEN_DUPLICATE |
                                            TOKEN_ADJUST_DEFAULT |
                                            TOKEN_QUERY |
                                            TOKEN_ASSIGN_PRIMARY,
                                            &token_cur)) {
    if (DuplicateTokenEx(token_cur, 0, NULL, SecurityImpersonation,
                         TokenPrimary, &token_dup)) {
      if (adjust_token_integrity_level(token_dup, requested_sid))
        launch_notepad_as_user(token_dup);
      CloseHandle(token_dup);
    }
    CloseHandle(token_cur);
  }
}