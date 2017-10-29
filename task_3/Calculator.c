#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define N 100000000 //число точек

//функция  f = x^2, отрезок [a; b] = [0; 5], область значений [min; Max] = [0; 25]
//для каждого потока есть свой элемент массива, поэтому без синхронизации
double a, b, min, Max;
int *array;
int n; //число потоков

typedef struct {
	int num;
} input_t;


void * generate(void * d);


int main() {
	a = 0;
	b = 5;
	min = 0;
	Max = 25;
	int i;
	printf("Enter the number of threads\n");
	scanf("%d", &n);
	pthread_t *thread = (pthread_t *) malloc (n * sizeof(pthread_t));
	srand(time(NULL));
	input_t *t = (input_t *) malloc (n * sizeof(input_t));

	int shmid;
	char pathname[] = "Calculator.c";
	key_t key;
	if ((key = ftok(pathname,0)) < 0) {
		printf("Can't generate key\n");
		exit(-1);
	}
	if ((shmid = shmget(key, n * sizeof(int), 0666|IPC_CREAT|IPC_EXCL)) < 0) {
		if (errno != EEXIST) {
			printf("Can't create shared memory\n");
			exit(-1);
		} else {
		if ((shmid = shmget(key, n * sizeof(int), 0)) < 0) {
			printf("Can't find shared memory\n");
			exit(-1);
			}
		}
	}
	array = (int*)shmat(shmid, NULL, 0);
	if (array == (int*) (-1)) {
		printf("Can't attach shared memory\n");
		exit(-1);
	}

	for (i = 0; i < n; i++) {
		array[i] = 0;
	}



	for (i = 0; i < n; i++) {
		t[i].num = i;
		int st =pthread_create (&thread[i], (pthread_attr_t * )NULL, generate, (void*)&t[i]);
		if (st != 0) {
			printf("Thread can't be created\n");
			exit(-1);
		}
	}
	
	for (i = 0; i < n; i++) {
		pthread_join(thread[i], (void **)NULL);
	}
	if (shmdt(array) < 0) {
		printf("Can't detach shared memory\n");
		exit(-1);
	}
	free(thread);
	free(t);
	return 0;
}

void * generate(void * d) {
	double x, y, y1;
	long long j, jmax;
	unsigned int seed = time(NULL);
	input_t * data = (input_t *)d;
	int i = data->num;
	if (i != 0) {
		jmax = N / n;
	} else {
		jmax = (N / n) + (N % n);
	}
	int k = 0;
	for (j = 0; j < jmax ; j++) {
		x = (double)(rand_r(&seed)) / RAND_MAX;
		x = (x * (b - a)) + a;
		y = (double)(rand_r(&seed))/ RAND_MAX;
		y = (y * (Max - min)) + min;
		y1 = x * x;
		if (y <= y1) {
			k ++;
		}
	}
	array[i] = k;
	
	return 0;
}

