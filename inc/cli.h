#ifndef _CLI_H
#define _CLI_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include "outils.h"
#include "debug.h"
#include "pol.h"
#include "matrix.h"


void trimstr(char * str);
char ** splitcommand(char * buffer);
char * readuserinput();


#endif