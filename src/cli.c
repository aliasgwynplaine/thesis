#include "cli.h"

#define MAX_STDINPUT_LENGTH 1024
#define MAX_ARG_NUM 64

void trimstr(char * str) {
    int len = strlen(str);
    if (str[len - 1] == '\n') str[len - 1] = '\0';
}

char ** splitcommand(char * buffer) {
    char ** splitted = malloc(sizeof(char *) * MAX_ARG_NUM);
    TESTPTR(splitted);
    
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
            FREE(line);
            FREE(cmds);
            continue;
        }
        
        if (strcmp(cmds[0], "set") == 0) {
            printf("setting prelude variable\n");
            if (strlen(cmds[1]) >= 32) {
                debug("prelude is too long!!! let's cut!");
                cmds[1][31] = '\0';
            }
            strcpy(prelude, cmds[1]);
            printf("%s> ", prelude);
        }

        if (strcmp(cmds[0], "exit") == 0) {
            printf("goodbye!\n");
            FREE(line);
            FREE(cmds);
            FREE(prelude);
            return 0;
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
            int c = expcmp_lex(x, y, n);
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
            llpol = llpol_malloc(n);
            llpol = addterm_llpol(llpol, 1, 3);
            llpol = addterm_llpol(llpol, 1, 3);
            llpol = addterm_llpol(llpol, 1, 1);
            llpol = addterm_llpol(llpol, 1, 2);
            llpol = addterm_llpol(llpol, 1, 3);
            llpol = addterm_llpol(llpol, 1, 7);
            llpol = addterm_llpol(llpol, 1, 7);
            llpol = addterm_llpol(llpol, 1, 7);
            printllpol(llpol);
            llpol_free(llpol);
            
        }

        if (strcmp(cmds[0], "aapol") == 0) {
            debug("running pol test...");
            u8 n = 1;
            aapol = aapol_malloc(n);
            debug("adding terms...");
            addterm_aapol(aapol, 1, 3);
            addterm_aapol(aapol, 1, 3);
            addterm_aapol(aapol, -1, 1);
            addterm_aapol(aapol, 1, 2);
            addterm_aapol(aapol, 1, 3);
            addterm_aapol(aapol, 1, 7);
            addterm_aapol(aapol, 1, 7);
            addterm_aapol(aapol, 1, 7);
            addterm_aapol(aapol, 1, 7);
            addterm_aapol(aapol, 1, 2);
            addterm_aapol(aapol, 1, 1);
            addterm_aapol(aapol, -1, 7);
            addterm_aapol(aapol, -1, 7);
            addterm_aapol(aapol, 7, 0);
            printaapol(aapol);
            debug("ordering aapol...");
            aapol_sort(aapol);
            debug("pol->sz: %d", aapol->sz);
            printaapol(aapol);
            printf("Second sort...\n");
            aapol_sort(aapol);
            printaapol(aapol);
            aapol_free(aapol);
            
        }

        if (strcmp(cmds[0], "pol2mat") == 0) {
            debug("running polynomial to matrix transformation");
            aapol1 = aapol_malloc(1);
            aapol2 = aapol_malloc(1);
            aapol3 = aapol_malloc(1);
            addterm_aapol(aapol1, 1, 5);
            addterm_aapol(aapol1, 1, 3);
            addterm_aapol(aapol1, 1, 3);
            addterm_aapol(aapol1, 1, 2);
            addterm_aapol(aapol2, 1, 0);
            addterm_aapol(aapol2, 1, 1);
            addterm_aapol(aapol2, 1, 3);
            addterm_aapol(aapol3, 1, 5);
            addterm_aapol(aapol3, 1, 2);
            addterm_aapol(aapol3, 1, 1);
            addterm_aapol(aapol3, 7, 0);
            debug("pol is created.");
            aapol_sort(aapol1);
            aapol_sort(aapol2);
            aapol_sort(aapol3);
            printaapol(aapol1);
            printaapol(aapol2);
            printaapol(aapol3);
            laapol = malloc(3 * sizeof(aapol_t));
            laapol[0] = *aapol1;
            laapol[1] = *aapol2;
            laapol[2] = *aapol3;
            mmatrix_t * smat = aapol2mmatrix(laapol, 3);
            debug("convertion done. freeing pols");
            aapol_free(aapol1);
            aapol_free(aapol2);
            aapol_free(aapol3);
            free(laapol);
            mmatrix_free(smat);
            debug("done.");
        }

        if (strcmp("bst", cmds[0]) == 0) {
            printf("\nempty\n");
        }
        
        FREE(line);
        FREE(cmds);
    }

    return 0;
}

int main (int argc, char * argv[]) {
    int state = cli("prelude");
    return state;
}