#include "cli.h"

#define MAX_STDINPUT_LENGTH 1024
#define MAX_ARG_NUM 64

bstree_t * setoaapol;
bstree_t * setollpol;

void trimstr(char * str) {
    int len = strlen(str);
    if (str[len - 1] == '\n') str[len - 1] = '\0';
}

char ** splitcommand(char * buffer) {
    char ** splitted = malloc(sizeof(char *) * MAX_ARG_NUM);
    CHECKPTR(splitted);
    
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
    //setoaapol = bstree_create(aapol_monomial_cmp, aapol_malloc, aapol_free);
    char * line;
    char ** cmds;

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