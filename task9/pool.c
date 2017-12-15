#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include "pool.h"

int task1(int a) {
	return a + 1;
}

int task2(int a) {
	return a + 2;
}

int task3(int a) {
	return a + 3;
}

int main() {
	int n, num;
	int i, s, j;
	Data x;
	printf("Enter the number of threads\n");
	scanf("%d", &num);
	printf ("Enter the number of tasks\n");
	scanf("%d", &n);
	int * jid = (int*) malloc (n * sizeof(int));
	struct List * joblist = job_list_create();
	printf("Enter the tasks in time order. Format: argument (integer), priority (any natural number, type (must be 1, 2 or 3)\n");
	for (i = 0; i < n; i++) {
		scanf("%d %d %d", &x.arg, &x.pr, &s);
		switch(s) {
			case 1:
				x.act = &task1;
				break;
			case 2:
				x.act = &task2;
				break;
			case 3:
				x.act = &task3;
				break;
		}
		jid[i] = thread_pool_add_job(joblist, x);
	}
	pthread_t * thread = thread_pool_start(num, joblist);
	thread_pool_destroy(thread, num);
	
	job_list_destroy(joblist);
	return 0;
}

