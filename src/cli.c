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
    aapol_t * aapol1;
    aapol_t * aapol2;
    aapol_t * aapol3;
    aapol_t * laapol;

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
            u8  n;
            printf("exponent test\n");
            printf("unpacking.\n");
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
            printf("packing.");
            printf("e: ");
            for (int i = 0; i < n; i++) {
                printf("%ld ", *(e + i));
            }
            printf("\n");
            u64 a = packexp(e, n);
            printf("x: %ld\n", a);
            

            FREE(e);
            FREE(e2);
            FREE(e3);
        }

        if (strcmp(cmds[0], "llpol") == 0) {
            debug("running pol test...");
            u8 n = 4;
            llpol = llpolmalloc(n);
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
            u8 n = 1;
            aapol = aapolmalloc(n);
            debug("adding terms...");
            addterm2aapol(aapol, 1, 3);
            addterm2aapol(aapol, 1, 3);
            addterm2aapol(aapol, 1, 1);
            addterm2aapol(aapol, 1, 2);
            addterm2aapol(aapol, 1, 3);
            addterm2aapol(aapol, 1, 7);
            addterm2aapol(aapol, 1, 7);
            addterm2aapol(aapol, 1, 7);
            addterm2aapol(aapol, 1, 7);
            addterm2aapol(aapol, 1, 2);
            addterm2aapol(aapol, 1, 1);
            addterm2aapol(aapol, 1, 7);
            addterm2aapol(aapol, 1, 7);
            addterm2aapol(aapol, 7, 0);
            printaapol_t(aapol);
            debug("ordering aapol...");
            sortaapol_t(aapol);
            printaapol_t(aapol);
            printf("Second sort...\n");
            sortaapol_t(aapol);
            printaapol_t(aapol);
            freeaapol_t(aapol);
            
        }

        if (strcmp(cmds[0], "pol2mat") == 0) {
            debug("running polynomial to matrix transformation");
            aapol1 = aapolmalloc(1);
            aapol2 = aapolmalloc(1);
            aapol3 = aapolmalloc(1);
            addterm2aapol(aapol1, 1, 34);
            addterm2aapol(aapol1, 1, 5);
            addterm2aapol(aapol1, 1, 2);
            addterm2aapol(aapol1, 1, 313);
            addterm2aapol(aapol2, 1, 0);
            addterm2aapol(aapol2, 1, 1);
            addterm2aapol(aapol2, 1, 3);
            addterm2aapol(aapol3, 1, 654);
            addterm2aapol(aapol3, 1, 1);
            addterm2aapol(aapol3, 1, 536);
            addterm2aapol(aapol3, 7, 0);
            debug("pol is created.");
            sortaapol_t(aapol1);
            sortaapol_t(aapol2);
            sortaapol_t(aapol3);
            printaapol_t(aapol1);
            printaapol_t(aapol2);
            printaapol_t(aapol3);
            laapol = malloc(3 * sizeof(aapol_t));
            laapol[0] = *aapol1;
            laapol[1] = *aapol2;
            laapol[2] = *aapol3;
            aapol2matrix(laapol, 3);
            debug("convertion done. freeing pols");
            // freeaapol_t(aapol1);
            // freeaapol_t(aapol2);
            // freeaapol_t(aapol3);
            freeaapol_t(laapol);
            debug("done.");
        }

        if (strcmp("bst", cmds[0]) == 0) {
            setoint_t * s = setoint_create();
            int d = 1;
            
            while (d > 0) {
                printf("insert data: ");
                scanf("%d", &d);
                setoint_insert(s, d);
            }
            
            debug("Dumping set of ints...");
            u64 * dump = setoint_dump(s);
            printf("Now, here it is: \n");
            
            for (int i = 0; i < s->sz; i++) {
                printf("%ld ", *(dump + i));
            }
            
            FREE(dump);
            setoint_free(s);
            printf("\nDone!\n");
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