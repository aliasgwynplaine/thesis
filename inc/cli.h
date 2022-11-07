#ifndef _CLI_H
#define _CLI_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>


void trimstr(char * str);
char ** splitcommand(char * buffer);
char * readuserinput();


#endif