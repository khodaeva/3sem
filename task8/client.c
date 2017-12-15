#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

typedef struct {
	int id;
	int socket;
	struct sockaddr_in serv;
	char * ip;
	int port;
	int res;
} input_t;

int num; //число серверов

#define N 100000000 //всего точек

void * mythread (void* p1) {
	input_t * p = (input_t *)p1;
	int i = p->id;
	int PORT = p->port;
	int j, jmax;
	if (i != 0) {
		jmax = N / num;
	} else {
		jmax = (N / num) + (N % num);
	}
	struct sockaddr_in servaddr = p->serv;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	if (inet_aton(p->ip, &servaddr.sin_addr) == 0) {
		printf("Invalid ip address number %d\n", i);
		exit(-1);
	}
	if (connect(p->socket, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		perror("Connect error");
		exit(-1);
	}
	int a;
	if ((a = send(p->socket, &jmax, sizeof(jmax), 0)) < 0){
		printf("Send fail thread %d\n", i);
		exit(-1);
	}
	int res = 0;
	if ((a = recv(p->socket, &res, sizeof(int), 0)) < 0) {
		printf("%d\n", a);			
		printf("Can't read thread %d\n", i);
		exit(-1);
	}
	p->res = res;
}

int main(int argc, char *argv[]) {
	double a = 0;
	double b = 5;
	double min = 0;
	double Max = 25;

	int sockfd;
	num  = (argc - 1) / 2;
	struct sockaddr_in * servaddr = (struct sockaddr_in *) malloc (num * sizeof(struct sockaddr_in));
	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Can't create socket");
		exit(-1);
	}
	pthread_t * thread = (pthread_t *) malloc (num * sizeof(struct sockaddr_in));
	input_t * input = (input_t *) malloc (num * sizeof(input_t));
	int i;
	for (i = 0; i < num; i++) {
		input[i].id = i;
		input[i].socket = sockfd;
		input[i].serv = servaddr[i];
		input[i].ip = argv[i + 1];
		input[i].port = atoi(argv[num + i + 1]);
	}
	for (i = 0; i < num; i++) {
		if (pthread_create(&thread[i], NULL, mythread, (void*)&input[i]) > 0) {
			printf("Thread create error\n");
			exit(-1);
		}
	}
	for (i = 0; i < num; i++) {
		pthread_join(thread[i], (void**)NULL);
	}
	int sum = 0;
	for (i = 0; i < num; i++) {
		sum += input[i].res;
	}
	free(input);
	free(servaddr);
	free(thread);
	printf("%f\n", (double)sum * (Max - min) * (b - a) / N);
	close(sockfd);
	return 0;
}
