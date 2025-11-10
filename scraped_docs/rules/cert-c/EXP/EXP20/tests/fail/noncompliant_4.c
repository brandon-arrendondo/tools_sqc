void login(char *usr, char *pw) {
  User user = find_user(usr);
  if (!strcmp((user->password),pw)) {
    grantAccess();
  }
  else {
    denyAccess("Incorrect Password");
  }
}