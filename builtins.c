/*
 *
 * tsh - tiny shell
 * builtin functions
 *
*/

#include <pwd.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

// define builtin functions
char *get_home_dir() {
    struct passwd *pw = getpwuid(getuid());
    return pw -> pw_dir;
}

int bi_cd(char **args){
    char *dir;

    if(args[1] == NULL || strcmp(args[1], "~") == 0){
        // no directory provided to change to, default is home directory
        dir = get_home_dir();
    } else {
        dir = args[1];
    }

    if (chdir(dir) != 0) {
        perror("cd");
    }

    return 1;
}

int bi_exit(char **args) {
    return 0;
}
