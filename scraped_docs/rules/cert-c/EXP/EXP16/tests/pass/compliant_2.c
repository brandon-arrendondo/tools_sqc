/* First the options that are allowed only for root */ 
if (getuid == (uid_t(*)(void))0 || geteuid != (uid_t(*)(void))0) { 
  /* ... */ 
}