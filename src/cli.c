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
    if (feof(stdin)) { printf("eof!\n"); FREE(buffer); exit(0); }

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
            FREE(line);
            FREE(cmds);
            FREE(prelude);
            break;
        }

        if (strcmp(cmds[0], "ecmp") == 0) {
            u64 x, y;
            u8 n;
            printf("exponent test\n");
            printf("enter x: ");
            scanf("%ld", &x);
            printf("enter y: ");
            scanf("%ld", &y);
            printf("enter n: ");
            scanf("%hhd", &n);
            printf("x: ");
            u64 * e = unpackexp(x, n);
            for (int i = 0; i < n; i++) {
                printf("%ld ", *(e + i));
            }
            printf("\ny: ");
            u64 * e2 = unpackexp(y, n);
            for (int i = 0; i < n; i++) {
                printf("%ld ", *(e2 + i));
            }
            printf("\n");
            int c = cmpexplex(x, y, n);
            if (c == 0) {
                printf("x = y\n");
            } else if (c < 0) printf("x < y\n");
            else printf("x > y\n");
            printf("x + y: ");
            for (int i = 0; i < n; i++) {
                printf("%ld ", *(e+i) + *(e2 + i));
            }
            printf("\nx + y = %ld\n", x+y);
            u64 * e3 = unpackexp(x+y, n);
            printf("x + y: ");
            for (int i = 0; i < n; i++) {
                printf("%ld ", *(e3+ i));
            }
            printf("\n");

            FREE(e);
            FREE(e2);
            FREE(e3);
        }

        if (strcmp(cmds[0], "run") == 0) {
            debug("running exp test...");
            u64 a   = 844489355100167;
            u64 b   = 844454995361799;
            u8 n    = 4;
            u64 * e = unpackexp(a, n);
            printf("%ld : ", a);
            for (int i = 0; i < n; i++) {
                printf("%ld ", *(e + i));
            }
            e = unpackexp(b, n);
            printf("%ld : ", b);
            for (int i = 0; i < n; i++) {
                printf("%ld ", *(e + i));
            }

            if (cmpexplex(a, b, n)) {
                printf("a > b\n");
            } else printf("a < b\n");

            printf("\n");
            FREE(e);
            debug("done!");
            exit(1);
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
            printpol(pol);
            debug("done!");
            pol_t * pol2 = NULL;
            pol2 = polnvars(8);
            
            if (!pol2) return 1;
            
            pol2 = addterm2pol(pol2, 1, -1);
            pol2 = addterm2pol(pol2, 31.2, -2);
            pol2 = addterm2pol(pol2, 3.41, 11198612397642394);

            printpol(pol2);
            //pol2 = addterm2pol(pol2, 123, 9873);
            //pol2 = addterm2pol(pol2, .002, 31);
            //printpol(pol2);
            debug("creating empty pol and trying to print it.");
            // pol_t * epol = NULL;
            // printpol(epol);
            // epol = addterm2pol(epol, 39.2, 200);
            // printpol(epol);
            debug("freeing the pols...");
            freepol_t(pol);
            freepol_t(pol2);
            debug("done!");
            exit(0);
        }

        /*
        
        set the variables here

        */
        //int status;
        // int rc = fork();
        // if (rc == 0) { //proceso hijo
        //     int pos = 0;
        //     execvp(cmds[0], cmds);
        // }
        // else {// proceso padre
        //     wait(NULL);
        //     printf("%s> ", prelude);
        // }

        FREE(line);
        FREE(cmds);
    }
    return 0;
}

int main (int argc, char * argv[]) {
    int state = cli("prelude");
    return state;
}