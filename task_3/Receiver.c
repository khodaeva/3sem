#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

int n; //число потоков
#define N 100000000 //число точек
int main(){
	double a = 0;
	double b = 5;
	double min = 0;
	double Max = 25;
	printf("Enter the number of threads\n");
	scanf("%d", &n);

	int * array;
	int i;
	int shmid;
	char pathname[] = "Calculator.c";
	key_t key;
	if ((key = ftok(pathname,0)) < 0) {
		printf("Can't generate key\n");
		exit(-1);
	}
	if ((shmid = shmget(key, n*sizeof(int), 0)) < 0) {
			printf("Can't find\n");
			exit(-1);
	}
	if ((array = (int*)shmat(shmid, NULL, 0)) == (int*) (-1)) {
		printf("Can't attach\n");
		exit(-1);
	}

	int sum = 0;
	for (i = 0; i < n; i++) {
		sum += array[i];
	}
	if (shmdt(array) < 0) {
		printf("Can't detach shared memory\n");
		exit(-1);
	}
	int status = shmctl(shmid, IPC_RMID, NULL);//удалим разделяемую память из системы
	printf("%f\n", (double)sum * (Max - min) * (b - a) / N);
	return 0;
}





