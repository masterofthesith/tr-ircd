/* Password Generator for TR-IRCD.
 * Copyright (c)2000-2003 TR-IRCD Development
 */

#include "struct.h"
#include "s_user.h"
#include "s_conf.h"
/* 
 * Header of the original mkpasswd.c:
 * simple password generator by Nelson Minar (minar@reed.edu)
 * copyright 1991, all rights reserved.
 * You can use this code as long as my name stays with it.
 */

struct server_info ServerInfo;

int main(int parc, char *parv[])
{
  char passarr[PASSWDLEN];
  char *input;
  char *ptpasswd;  
  int i;
  
  if (parc != 1) 
    return printf("\nUsage: ircpasswd\n");

  input = (char *)malloc(PASSWDLEN);
  memset(input, 0, PASSWDLEN);
  fprintf(stdout, "Please enter plain text password: ");
  fgets(input, PASSWDLEN, stdin);
  if(strlen(input) <=1 ) {
	  fprintf(stdout, "\nPlease think of a proper plain text password and try again!\n");
	  free(input);
	  return 0;
  }
  
  ptpasswd = (char *)malloc(strlen(input)-1);
  for(i = 0 ; i < strlen(input)-1 ; i++) 
	  ptpasswd[i] = input[i];
    
  printf("Encryption is: %s\n", calcpass(ptpasswd, passarr));

  free(input);
  free(ptpasswd);

  return 0;
}

