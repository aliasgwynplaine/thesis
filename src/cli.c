#include "cli.h"
#include "outils.h"
#include "debug.h"
/* here you should import your pollib */

#define MAX_STDINPUT_LENGTH 1024
#define MAX_ARG_NUM 64

char * trimstr(char * str) {
    int len = strlen(str);
    if (str[len - 1] == '\n') str[len - 1] = '\0';
    return str;
}

char ** splitcommand(char * buffer) {
    char * part = strtok(buffer, " ");
    char ** splitted = malloc(sizeof(char *) * MAX_ARG_NUM);

    if (splitted == NULL) {
        fprintf(stderr, "No memory!!\n");
        /* todo: how do i handle this??? */
    }
        int pos = 0;

    while (part != NULL) {
        *(splitted + pos) = part;
        part = strtok(NULL, " ");
        pos++;
    }

    *(splitted + pos - 1) = trimstr(*(splitted + pos - 1));
    *(splitted + pos) = NULL;

    return splitted;
}

char ** read_user_input() {
    char * buffer = malloc(sizeof(char) * MAX_STDINPUT_LENGTH);
    fgets(buffer, MAX_STDINPUT_LENGTH, stdin);
    if (feof(stdin)) {printf("eof!\n");return NULL;}
    //printf("feof: %i\n", feof(stdin));
    char ** commandv = splitcommand(buffer);

    return commandv;
}


int cli(char * prelude_str) {
    char * prelude = (char*) malloc(sizeof(char) * 32);
    strcpy(prelude, prelude_str);
    printf("%s> ", prelude);
    char ** command;

    while (1) {
        command = read_user_input();
        debug("command readed");
        if (!command) {
            printf("%s> ", prelude);
            continue;
        }
        print_lstr(command);
        
        if (strcmp(command[0], "set") == 0) {
            printf("setting prelude variable\n");
            strcpy(prelude, command[1]);
            printf("%s> ", prelude);
            continue;
        }

        /*
        
        set the variables here

        */

        int rc = fork();
        if (rc == 0) { //proceso hijo
            int pos = 0;
            execvp(command[0], command);
        }
        else {// proceso padre
            wait(NULL);
            printf("%s> ", prelude);
        }
    }
}

int main (int argc, char * argv[]) {
    int state = cli("prelude");
    return state;
}