#ifndef _CLI_H
#define _CLI_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


char * trimstr(char * str);
char ** splitcommand(char * buffer);
char ** read_user_input();


#endif