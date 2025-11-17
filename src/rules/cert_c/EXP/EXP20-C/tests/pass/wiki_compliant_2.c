/*
 * Rule: EXP20-C
 * Source: wiki
 * Status: PASS - Should NOT trigger EXP20-C violation
 */

int validateUser(User usr) {
  if(list_contains(validUsers, usr)) {
    return 1;
  }

  return 0;
}

void processRequest(User usr, Request request) {
  if(validateUser(usr) == 0) {
    return "invalid user";
  }
  else {
    serveResults();
  }
}