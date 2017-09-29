/*
  Write a program which allow you to: 
- run another programs via command line.
- get exit codes of terminated programs

## TIPS:
1. Use "2_fork_wait_exit.c" and "4_exec_dir.c" from examples. Combine them.
2. Parse input string according to the type of exec* (see "man exec").
   a) if execvp is used, string splitting into "path" and "args" is all you need.
3. Collect exit codes via waitpid/WEXITSTATUS.
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main()
{
	char c = 'a';
	char t;
	int argc, i, status;

	while (tolower(c) != 'n') {
		const pid_t pid = fork();
		if (pid == -1) {
			printf("fork error\n");
			return(-1);
		}
		if (pid != 0) {
			waitpid(pid, &status, 0);
			printf("Ret code: %d\nOnce again?(y/n)\n", WEXITSTATUS(status));
			c = getchar();
		} else {
			char ** arg;
			arg = (char **) malloc (1 * sizeof(char *));
			i = 0;
			do {
				arg[i] = (char*) malloc (100 * sizeof(char));
				scanf("%s", arg[i]);
				i++;
				arg = (char **) realloc (arg, (i + 1) * sizeof(char*));
			} while (getchar() != '\n');
			argc = i;
			arg[argc] = NULL;
			execvp(arg[0], arg);
			for (i = 0; i < (argc + 1); i++) {
				free(arg[i]);
			}
			free(arg);
		}
	}
	return 0;
}
			
