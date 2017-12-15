#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex;

typedef struct {
	int arg;
	int pr; //priority
	int (* act) (int);
	int res;
	int num;
} Data;
struct Node {
	Data val;
	struct Node * next;
};
struct List {
	struct Node * head;
};


struct List * job_list_create ()
{
	struct List * s = (struct List *) malloc (1 * sizeof(struct List));
	s->head = NULL;
	return s;
}

int thread_pool_add_job (struct List * list, Data x)
{
	int i = 1;
	struct Node * p;
	if (list->head ==NULL)
	{
		list->head = (struct Node *) malloc (1 * sizeof(struct Node));
		p = list->head;
		p->val = x;
		p->next = NULL;
		(p->val).num = 1;
		return 1;
	}
	else
	{
		p = list->head;
		while (p->next != NULL)
		{
		p = p->next;
		i++;
		}
		p->next = (struct Node *) malloc (1 * sizeof(struct Node));
		i++;
		p = p->next;
		p->val = x;
		p->next = NULL;
		(p->val).num = i;
		return i;
	}
}

int findMax (struct List * list)
{
	struct Node * p;
	p = list->head;
	int max = 0;
	while (p !=  NULL) {
		if ((p->val).pr > max) {
			max = (p->val).pr;
		}
		p = p->next;
	}
	return max;
}

Data extractMax (struct List * list)
{
	int max;
	Data x;
	max = findMax(list);
	struct Node * p;
	struct Node * p_pre;
	struct Node * p_next;
	p = list->head;
	if (p == NULL) {
		//очередь уже пуста
		x.arg = 0;
		x.pr = 0;
		x.act = NULL;
		return x;
	}
	p_next = p->next;
	while ((p->val).pr != max) {
			p_pre = p;
			p = p->next;
			p_next = p->next;
	}
	if (p != list->head) {
		p_pre->next = p_next;
	} else {
		list->head = p_next;
	}
	x = p->val;
	free(p);
	return x;
}


void list_print (struct List * list)
{
    if (list->head != NULL)
    {
        struct Node * p;
        p = list->head;
        while (p->next != NULL)
        {
            printf("%d \n", (p->val).num);
            p = p->next;
        }
        printf("%d \n", (p->val).num);
    }
    else
    {
        printf("Empty list\n");
    }
}

void list_clear(struct List * list)
{
    struct Node * p;
    struct Node * m;
    if (list->head != NULL)
    {
        p = list->head;
        while (p->next != NULL)
        {
            m = p->next;
            free(p);
            p = m;

        }
        free(p);
    }
}

void job_list_destroy (struct List * list)
{
    list_clear(list);
    free(list);
}

void * mythread (void * list) {
	int t;
	pthread_mutex_lock(&mutex);
	Data x = extractMax((struct List *)list);
	pthread_mutex_unlock(&mutex);
	while (x.pr != 0) {
		x.res = x.act(x.arg);
		printf("Task %d done res %d\n", x.num, x.res);
		pthread_mutex_lock(&mutex);
		x = extractMax(list);
		pthread_mutex_unlock(&mutex);
	}
}
pthread_t * thread_pool_start(int n, struct List *list) {
	pthread_mutex_init(&mutex, NULL);
	pthread_t * thread = (pthread_t *) malloc (n * sizeof(pthread_t));
	int i;
	for (i = 0; i < n; i++) {
		pthread_create(&thread[i], NULL, mythread, (void*)list);
	}
	return thread;
}

void thread_pool_destroy (pthread_t * t, int n) {
	int i;
	for (i = 0; i < n; i++) {
		pthread_join(t[i], (void**)NULL);
	}
	free(t);
}

