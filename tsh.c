#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <strings.h>

#include "builtins.h"

/*
 *
 * tsh - Tiny SHell
 * main file
 *
 * written by Ben Tomsett (100310064)
 * for coursework at the University of East Anglia
 * module: CMP-5013A - Architectures and Operating Systems
 * yru20qyu@uea.ac.uk
 *
*/

/*
 * Prints a prompt for the user to enter a command.
 * Gets the current working directory and outputs the current folder.
 */
void tsh_prompt() {
    char cwd[1024];

    // gets the current working directory of the shell
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("tsh");
        exit(EXIT_FAILURE);
    }

    char *folder;
    if (strcmp(cwd, get_home_dir()) == 0) {
        folder = "~";
    } else if (strcmp(cwd, "/") == 0) {
        folder = "/";
    } else {
        folder = strrchr(cwd, '/') + 1;
    }

    // print the prompt
    printf("%s $ ", folder);
}

/*
 * Reads in and returns a command entered by the user in the console.
 */
char *tsh_getline() {
    // definitions
    char *line; // the string being read in
    size_t size = 0; // setting the size of the buffer to 0 tells getline to manage the buffer size for us
    getline(&line, &size, stdin); // read in a line from stdin

    if (line == -1) {
        perror("tsh"); // getline returns -1 if error
        exit(EXIT_FAILURE);
    } else {
        return line;
    }
}

/*
 * Takes a line entered by the user, and splits it up into a list of arguments
 * in order to be executed by the shell.
 */
char **tsh_parseline(char *line) {
    int array_size = 128; // should be large enough that we don't have to realloc, but we can if needed
    char **tokens = malloc(array_size * sizeof(char *)); // buffer array of tokens
    char *token;

    if (!tokens) {
        perror("tsh"); // unable to allocate memory
        exit(EXIT_FAILURE);
    }

    int position = 0;
    token = strtok(line,
                   " \t\n"); // splits a string up (delimited by space, tab or newline), returns a pointer to the first token
    while (token != NULL) {
        tokens[position] = token; // store the token in the array of tokens
        position++;

        if (position >= array_size) { // exceeded the buffer, reallocate with extra memory
            array_size += 128;
            tokens = realloc(tokens, array_size * sizeof(char *));
            if (!tokens) {
                perror("tsh"); // unable to allocate memory
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, " \t\n"); // gets the next token (strtok replaces the delimiters with NULL)
    }
    tokens[position] = NULL; // null-terminate the array of tokens
    return tokens;
}

/*
 * Takes a list of arguments (from a command entered by the user) and creates a new
 * process (forks the current process, then changes it to the specified one).
 *
 */
int tsh_fork(char **args) {
    pid_t pid;
    pid = fork(); // forks the current process into a new process

    char cat[1024] = "/usr/bin/"; // restrict commands to /usr/bin
    strcat(cat, args[0]); // add the requested command name onto /usr/bin

    int status;
    if (pid == 0) { // this is the new child process
        if (execv(cat, args) == -1) { // execute the new program
            perror("tsh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) { // error forking
        perror("tsh");
    } else { // this is the parent process
        do {
            waitpid(pid, &status, WUNTRACED); // wait for child process to finish
            // WUNTRACED allows parent to return when child stopped, as well as well as exiting or being killed
        } while (!WIFEXITED(status) && !WIFSIGNALED(status)); // keep waiting while child process is still running
    }

    return 1;
}

/*
 * Takes a list of arguments, and then either passes it to a builtin function,
 * or passes it to tsh_fork to launch another program.
 */
int tsh_execute(char **args) {
    if (strcmp(args[0], "cd") == 0) {
        return bi_cd(args);
    } else if (strcmp(args[0], "exit") == 0) {
        return bi_exit(args);
    }

    return tsh_fork(args);
}

/*
 * Main function, a loop which keeps the shell running and asking for commands.
 */
int main() {
    int status = 1;
    char *line;
    char **parsed;

    while(status) {
        tsh_prompt();
        line = tsh_getline();
        parsed = tsh_parseline(line);
        status = tsh_execute(parsed);

        free(line);
        free(parsed);
    }

    return EXIT_SUCCESS;
}
