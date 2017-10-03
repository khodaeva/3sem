/*
Write a duplex pipe implementation.
typedef struct {
  int txd[2]; 
  int rxd[2]; 
} dpipe_t;
1) This pipe should connect some process with his child, for continuous communication.
2) Be careful with opened descriptors.
3) When one process is terminated, the other should also exit.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>

int main() {
	typedef struct {
		int txd[2]; //parent writes
		int rxd[2]; //child writes
	} dpipe_t;
	dpipe_t fd;
	size_t size;
	pid_t result, exited;
	int status;
	char for_parent[15];
	char for_child[16];
	if (pipe(fd.txd) < 0) {
		printf("Can't create pipe 1\n");
		exit(-1);
	}
	if (pipe(fd.rxd) < 0) {
		printf("Can't create pipe 2\n");
		exit(-1);
	}
	result = fork(); 
	if (result < 0) {
		printf("Fork error\n");
		exit(-1);
	} else if (result > 0) {
		//parent runs
		close(fd.txd[0]);
		size = write(fd.txd[1], "I'm your father", 16);
		if (size != 16) {
			printf("Parent can't send all message\n");
			exit(-1);
		}
		close(fd.txd[1]);
		close(fd.rxd[1]);
		size = read(fd.rxd[0], for_parent, 15);
		if (size < 0) {
			printf("Parent can't read message\n");
			exit(-1);
		}
		exited = waitpid(result, &status, 0);
		if (WEXITSTATUS(status) != 0) {
			printf("Child exited\n");
			exit(-1); //if child exited with -1
		}
		printf("To parent: %s\n", for_parent);
		close(fd.rxd[0]);
	} else {
		//child runs
		close(fd.rxd[0]);
		if (getppid() == 1) {
			printf("Parent exited\n");
			exit(-1); //if parent is terminated
		}
		size = write(fd.rxd[1], "I'm your child", 15);
		if (size != 15) {
			printf("Child can't send all message\n");
			exit(-1);
		}
		close(fd.rxd[1]);
		close(fd.txd[1]);
		if (getppid() == 1) {
			printf("Parent exited\n");
			exit(-1); //if parent is terminated
		}

		size = read(fd.txd[0], for_child, 16);
		if (size < 0) {
			printf("Child can't read message\n");
			exit(-1);
		}
		printf("To child: %s\n", for_child);
		close(fd.txd[0]);
		exit(0);

	}
	return 0;
}
