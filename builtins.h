#ifndef TSH_BUILTINS_H
#define TSH_BUILTINS_H

// define builtin functions
char *get_home_dir();
int bi_cd(char **args);
int bi_exit(char **args);

char *builtin_cmds[] = {"cd", "exit"};
int (*builtin_funcs[])(char **) = {&bi_cd, &bi_exit};

int get_builtins_len(){
    return sizeof(builtin_cmds) / sizeof(builtin_cmds[0]);
}

#endif //TSH_BUILTINS_H
