#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

int ex, counter1;
char rec;
pid_t pid0;


void handler_chld(int nsig) {
	printf("Receive signal %d, child exited\n", nsig);
	ex = 1;
}

void handler_term (int nsig) {
	printf ("Receive signal %d, SIG_TERM\n", nsig);
	exit (-1);
}
	
int power2(int n) {
	int i, res;
	res = 1;
	for (i = 0; i < n; i++) {
		res = res * 2;
	}
	return res;
} 


void handler (int nsig) {
	pid_t pid1;
	pid1 = getpid();
	char a = 0;
	if (pid1 == pid0) {
		if (nsig == SIGUSR1) {
			a = power2(counter1);
			rec += a;
		}
	} 
}
	

int main(int argc, char * argv[]) {
	ex = 0;
	pid_t pid;
	pid0 = getpid();
	char * path_source = argv[1];
	char * path_dest = argv[2];
	int fd_source, fd_dest;
	int counter;
	char i = 1;
	fd_dest = open(path_dest, O_CREAT|O_RDWR|O_EXCL, 0666);
	if (fd_dest < 0) {
		printf("Can't create destination file, or it already exists\n");
		exit(-1);
	}
	fd_source = open(path_source, O_RDONLY);
	if (fd_source < 0) {
		printf("Can't open source file\n");
		exit(-1);
	}
	signal (SIGCHLD, handler_chld);
	signal (SIGTERM, handler_term);
	signal(SIGUSR1, handler);
	signal(SIGUSR2, handler);
	sigset_t set, set1;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGUSR2);

	pid = fork();
	sigprocmask(SIG_SETMASK, &set, NULL);
	if (pid == 0) {
		char send;
		int readbytes;
		while ((readbytes = read(fd_source, (void*)&send, 1)) > 0) {
			counter = 0;
			while (counter < 8) {
				sigprocmask(SIG_SETMASK, &set, NULL);
				if ((send & i) == 1) {
					kill(pid0, SIGUSR1);
				} else {
					kill(pid0, SIGUSR2);
				}
				send = send >> 1;
				counter ++;
				sigsuspend(&set1);
			}
			
		}
		if (readbytes < 0) {
			printf("Can't read\n");
			exit(-1);
		}
		close(fd_source);
		exit(-1);
	} else {
		if (pid > 0) {
			int writebytes;
			rec = 0;
			counter1 = 0;
			while (ex == 0) {
				sigsuspend(&set1);
				sigprocmask(SIG_SETMASK, &set, NULL);
				counter1 ++;
				if (counter1 == 8) {
					write(fd_dest, (void*)&rec, 1);
					if (writebytes < 0) {
						printf("Can't write\n");
						exit(-1);
					}
					counter1 = 0;
					rec = 0;
				}
				kill(pid, SIGUSR1);
			}
			close(fd_dest);
			exit(-1);

		} else {
			printf("Can't fork\n");
			exit(-1);
		}
	} 
	return 0;
}
