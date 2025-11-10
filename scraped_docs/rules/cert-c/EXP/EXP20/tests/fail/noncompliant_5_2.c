int check_password(User *user, char *pw_given) {
  if (!strcmp((user->password),pw_given)) {
    return 1;
  }
  return 0;

}

void login(char *usr, char *pw) {
  User user = find_user(usr);
  if (!check_password(user, pw)) {
    grantAccess();
  }
  else {
    denyAccess("Incorrect Password");
  }
}