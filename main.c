
/**************************************************************
* main.c
* Funktion: Minishell.
* Autor: René Beiermann, Emma Wegner
*************************************************************/

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>


char *s_isBuildIn[] = {
        "cd",
        "exit",
        "printenv"
};

int s_cd(char **args) {
    if (args[1] == NULL) printf("Bitte Ordner eingeben");
    else {
        //cd
        chdir(args[1]);
    }
    return 1;
}

int s_exit() {
    return 0;
}

int run(char **args) {
    pid_t pid;
    pid = fork();
    int stat;
    if (pid == 0)
        //In C ist alles was nicht 0 ist true. execvp returned int.
        if (execvp(args[0], args)) perror("Irgendwas ist schief gelaufen");
        else {
            do {
                waitpid(pid, &stat, WUNTRACED);
            } while (!WIFEXITED(stat) && !WIFSIGNALED(stat));

            //waitpid schreibt status in stat. da kommt 0 oder true bei raus.
            //#define	WUNTRACED	2	/* Report status of stopped children.  */
        }
}

int runCommands(char **args, char *envp[]) {
    for (int i = 0; i < 3; i++) {
        //Compare S1 and S2.
        //extern int strcmp (const char *__s1, const char *__s2)
        if (strcmp(args[0], s_isBuildIn[i]) == 0) {
            if (i == 0) return s_cd(args);
            if (i == 1) return s_exit(args);
            if (i == 2) {
                for (i = 0; envp[i] != NULL; i++)
                    printf("\n%s", envp[i]);
                return 1;
            }
        }
    }
    return run(args);
}

char *getInput() {
    char *line = NULL;
    ssize_t buffer = 0;
    getline(&line, &buffer, stdin);
    return line;
}

char **splitting(char *line) {
    int pos = 0;
    /* Allocate SIZE bytes of memory.  */
    //extern void *malloc (size_t __size) __THROW __attribute_malloc__

    char **sentence = malloc(sizeof(line));
    char *word;

    word = strtok(line, " \t\r\n\a");
    while (word != NULL) {
        sentence[pos] = word;
        pos++;
        /* Divide S into tokens separated by characters in DELIM.  */
        //extern char *strtok (char *__restrict __s, const char *__restrict __delim)
        word = strtok(NULL, " \t\r\n\a");
    }
    sentence[pos] = NULL;
    return sentence;
}

void printPrompt() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\t", cwd);
        printf(" @%s", getenv("USER"));
        printf(" >>>> ");
    }
}


int main(int argc, char *argv[], char *envp[]) {

    char *line;
    char **args;
    int status;

    do {
        printPrompt();
        line = getInput();
        args = splitting(line);
        status = runCommands(args, envp);
    } while (status);
    return 0;
}
