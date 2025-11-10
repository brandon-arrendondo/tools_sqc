typedef void SighandlerType(int signum);
extern SighandlerType *signal(
  int signum,
  SighandlerType *handler
);