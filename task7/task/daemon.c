#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <linux/fs.h>
#include <signal.h>

#define MAX_EVENTS 1024
#define LEN_NAME 16
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (MAX_EVENTS * (EVENT_SIZE + LEN_NAME))

FILE * p;

void daemon_initialize() {
 
	pid_t pid;
	pid = fork();
	if (pid < 0) {
		printf("Can't fork\n");
		exit(-1);
	}
	if (pid > 0) {
		exit(0);
	}
	//записываем pid
	pid = getpid();
	FILE * file = fopen("pid.txt", "w");
	fprintf(file, "%d", pid);
	fclose(file);
	//игнорируем сигналы
	sigset_t set;
	sigemptyset(&set);
	sigfillset(&set);
	sigprocmask(SIG_SETMASK, &set, NULL);

	if (setsid() < 0) {
		printf("Can't create new group\n");
		exit(-1);
	}
	if (chdir("/") < 0) {
		printf("Can't enter the root directory\n");
		exit(-1);
	}
	
	open("/dev/null", O_RDWR);
	dup(0);
	dup(0);
}



void event_handle (struct inotify_event * ev, FILE * p) {
	if (ev->mask & IN_MODIFY) {
		fprintf(p, "time %ld: write in file %s\n", clock(), ev->name);
	}
	if (ev->mask & IN_DELETE) {
		if (ev->mask & IN_ISDIR) {
			fprintf(p, "time %ld: directory %s was deleted\n", clock(), ev->name);
		} else {
			fprintf(p, "time %ld: file %s was deleted\n", clock(), ev->name);
		}
	}
	if (ev->mask & IN_OPEN) {
		if (ev->mask & IN_ISDIR) {
			fprintf(p, "time %ld: opendir %s\n", clock(), ev->name);
		} else {
			fprintf(p, "time %ld: open file %s\n", clock(), ev->name);
		}
	}
	if (ev->mask & IN_CREATE) {
		if (ev->mask & IN_ISDIR) {
			fprintf(p, "time %ld: directory %s was created\n",clock(), ev->name);
		} else {
			fprintf(p, "time %ld: create file %s\n",clock(), ev->name);
		}
	}
	if (ev->mask & IN_ACCESS) {
		if (ev->mask & IN_ISDIR) {
			fprintf(p, "time %ld: readdir %s\n", clock(), ev->name);
		} else {
			fprintf(p, "time %ld: read file %s\n", clock(), ev->name);
		}
	}
	if (ev->mask & IN_CLOSE) {
		if (ev->mask & IN_ISDIR) {
			fprintf(p, "time %ld: closedir %s\n", clock(), ev->name);
		} else {
			fprintf(p, "time %ld: close file %s\n", clock(), ev->name);
		}
	}
}

int main(int argc, char * argv[]) {
	daemon_initialize();
	char * pathdir = argv[1];
	char * pathlog = argv[2];

	clock_t time;

	int fd, wd;
	char buf[BUF_LEN];
	int len, i;
	fd = inotify_init();
	if (fd < 0) {
		perror("inotify_init");
		exit(-1);
	}
	i = 0;
	pid_t pid2;
	pid2 = fork();
	struct inotify_event * event;
	if (pid2 == 0) {
		while(1) {
			i = 0;
			p = fopen(pathlog, "a");
			len = read(fd, buf, BUF_LEN);
			while (i < len) {
				event = (struct inotify_event *) &buf[i];
				event_handle(event, p);		
				i += sizeof(struct inotify_event) + event->len;
			}
			fclose(p);
			sleep(10);
		}
	} else {
		wd = inotify_add_watch(fd, pathdir, IN_ALL_EVENTS);
		while (1) { //inotify watch работает постоянно
		}
	}
	return 0;
}
