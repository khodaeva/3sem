#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#define LAST_MESSAGE 100
#define SIZE 1024

int main(int argc, char * argv[]) {
	int msqid;
	char pathname[] = "filecopy_sender.c";
	key_t key;
	int length, fd, i;
	char * filename = argv[1];
	size_t writebytes;
	struct mymsgbuf {
		long mtype;
		char mtext[SIZE];
		int bytes;
	} mybuf;
	if ((key = ftok(pathname, 0)) < 0) {
		printf("Can't generate key\n");
		exit(-1);
	}
	if ((msqid = msgget(key, 0666|IPC_CREAT)) < 0) {
		printf("Can't get msqid\n");
		exit(-1);
	}
	if ((fd = creat(filename, 0666)) < 0) {
		printf("Can't open file, or it already exists\n");
		exit(-1);
	}
	do {
		if ((length = msgrcv(msqid, (struct msgbuf *) &mybuf, SIZE + 5, 0, 0)) < 0) {
			printf("Can't receive message\n");
			exit(-1);
		}

		if ((writebytes = write(fd, (void*)&(mybuf.mtext), mybuf.bytes)) < 0) {
			printf("Can't write\n");
			exit(-1);
		}
	}
	while (mybuf.mtype != LAST_MESSAGE);
	msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
	if ((i = close(fd)) < 0) {
		printf("Can't close file\n");
		exit(-1);
	}
	return 0;
}






