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
    u64 * e;
    u64 * e2;
    u64 * e3;
    llpol_t * llpol;
    aapol_t * aapol;

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

        if (strcmp(cmds[0], "exp") == 0) {
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
            e = unpackexp(x, n);
            for (int i = 0; i < n; i++) {
                printf("%ld ", *(e + i));
            }
            printf("\ny: ");
            e2 = unpackexp(y, n);
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
            e3 = unpackexp(x+y, n);
            printf("x + y: ");
            for (int i = 0; i < n; i++) {
                printf("%ld ", *(e3+ i));
            }
            printf("\n");

            FREE(e);
            FREE(e2);
            FREE(e3);
        }

        if (strcmp(cmds[0], "llpol") == 0) {
            debug("running pol test...");
            u8 n = 4;
            llpol = llpolnvars(n);
            llpol = addterm2llpol(llpol, 0.31415, 2023451344);
            llpol = addterm2llpol(llpol, 1234124, 74834566);
            llpol = addterm2llpol(llpol, 981.134, 4576355545);
            llpol = addterm2llpol(llpol, 0.00011, 645674567);
            llpol = addterm2llpol(llpol, 0.645, 12023451344);
            llpol = addterm2llpol(llpol, 1234125334, 714834566);
            llpol = addterm2llpol(llpol, 981.1534534, 14576355545);
            llpol = addterm2llpol(llpol, 0.00345011, 1645674567);
            printllpol_t(llpol);
            freellpol_t(llpol);
            
        }

        if (strcmp(cmds[0], "aapol") == 0) {
            debug("running pol test...");
            u8 n = 4;
            aapol = aapolnvars(n);
            debug("adding terms...");
            aapol = addterm2aapol(aapol, 0.31415, 2023451344);
            aapol = addterm2aapol(aapol, 1234124, 74834566);
            aapol = addterm2aapol(aapol, 981.134, 4576355545);
            aapol = addterm2aapol(aapol, 0.00011, 645674567);
            aapol = addterm2aapol(aapol, 0.645, 12023451344);
            aapol = addterm2aapol(aapol, 1234125334, 714834566);
            aapol = addterm2aapol(aapol, 981.1534534, 14576355545);
            aapol = addterm2aapol(aapol, 0.00345011, 1645674567);
            printaapol_t(aapol);
            freeaapol_t(aapol);
            
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