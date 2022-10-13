#include "cli.h"
#include "outils.h"
#include "debug.h"
#include "pol.h"
/* here you should import your pollib */

#define MAX_STDINPUT_LENGTH 1024
#define MAX_ARG_NUM 64

void trimstr(char * str) {
    int len = strlen(str);
    if (str[len - 1] == '\n') str[len - 1] = '\0';
}

char ** splitcommand(char * buffer) {
    char ** splitted = malloc(sizeof(char *) * MAX_ARG_NUM);

    if (splitted == NULL) { 
        fprintf(stderr, "No memory!!\n");
        /* todo: how do i handle this??? */
        exit(EXIT_FAILURE);
    }
    
    char * part = strtok(buffer, " \n");
    int pos     = 0;

    while (part != NULL) {
        *(splitted + pos) = part;
        part = strtok(NULL, " \n");
        pos++;
    }

    //trimstr(*(splitted + pos - 1));
    *(splitted + pos) = NULL;

    return splitted;
}

char * readuserinput() {
    char * buffer = malloc(sizeof(char) * MAX_STDINPUT_LENGTH);
    fgets(buffer, MAX_STDINPUT_LENGTH, stdin);
    if (feof(stdin)) { printf("eof!\n"); free(buffer); exit(0); }

    return buffer;
}


int cli(char * prelude_str) {
    char * prelude = (char*) malloc(sizeof(char) * 32);
    strcpy(prelude, prelude_str);
    printf("%s> ", prelude);
    char * line;
    char ** cmds;

    while (1) {
        line = readuserinput();
        cmds = splitcommand(line);

        if (cmds[0] == NULL) {
            printf("%s> ", prelude);
            continue;
        }
        
        if (strcmp(cmds[0], "set") == 0) {
            printf("setting prelude variable\n");
            strcpy(prelude, cmds[1]);
            printf("%s> ", prelude);
            continue;
        }

        if (strcmp(cmds[0], "exit") == 0) {
            printf("goodbye!\n");
            free(line);
            free(cmds);
            free(prelude);
            return 0;
        }

        if (strcmp(cmds[0], "run") == 0) {
            debug("running exp test...");
            u64 a   = 1125925676908555;
            u8 n    = 4;
            u64 * e = unpackexp(a, n);
            printf("%ld : ", a);
            for (int i = 0; i < n; i++) {
                printf("%ld ", *(e + i));
            }
            printf("\n");
            free(e);
            debug("done!");
            debug("running pol test...");
            pol_t * pol = malloc(sizeof(pol_t));
            
            if (!pol) return 1;

            pol->coef = 1;
            pol->exp = 32;
            pol->nvar = 4;
            pol->nxt = NULL;
            debug("adding terms to pol...");
            pol = addterm2pol(pol, 2.3, 10);
            pol = addterm2pol(pol, -2., 20);
            pol = addterm2pol(pol, -.3, 21);
            pol = addterm2pol(pol, 201, -1);
            print_pol(pol);
            debug("done!");
            pol_t * pol2 = NULL;
            pol2 = polnvars(8);
            
            if (!pol2) return 1;
            
            pol2 = addterm2pol(pol2, 1, -1);
            pol2 = addterm2pol(pol2, 31.2, -2);
            pol2 = addterm2pol(pol2, 3.41, 11198612397642394);

            print_pol(pol2);
            //pol2 = addterm2pol(pol2, 123, 9873);
            //pol2 = addterm2pol(pol2, .002, 31);
            //print_pol(pol2);
            debug("creating empty pol and trying to print it.");
            pol_t * epol = NULL;
            print_pol(epol);
            epol = addterm2pol(epol, 39.2, 200);
            print_pol(epol);
            debug("freeing the pols...");
            free_pol_t(pol);
            free_pol_t(pol2);
            debug("done!");
            exit(0);
        }

        /*
        
        set the variables here

        */
        //int status;
        int rc = fork();
        if (rc == 0) { //proceso hijo
            int pos = 0;
            execvp(cmds[0], cmds);
        }
        else {// proceso padre
            wait(NULL);
            printf("%s> ", prelude);
        }

        free(line);
        free(cmds);
    }
}

int main (int argc, char * argv[]) {
    int state = cli("prelude");
    return state;
}