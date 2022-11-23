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

struct option {
    const char * long_name;
    char        short_name;
    int        has_arg : 1;
};


void trimstr(char * str);
char ** splitcommand(char * buffer);
char * readuserinput();


#endif