char *strcat(char *s1, const char *s2); 

char *c_str1 = "c_str1";
const char *c_str2 = "c_str2";
char c_str3[9] = "c_str3";
const char c_str4[9] = "c_str4";

strcat(c_str3, c_str2); 

/* Args reversed to prevent overwriting string literal */ 
strcat(c_str3, c_str1);  
strcat(c_str4, c_str3);  /* Compiler warns that c_str4 is const */