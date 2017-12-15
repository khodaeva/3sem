#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

//функция  f = x^2, отрезок [a; b] = [0; 5], область значений [min; Max] = [0; 25]
double a, b, min, Max;


int calculate(int * num) {
	int p = *num;
	double x, y, y1;
	int j;
	srand(time(NULL));
	int k = 0;
	for (j = 0; j < p ; j++) {
		x = (double)rand() / RAND_MAX;
		x = (x * (b - a)) + a;
		y = (double)rand() / RAND_MAX;
		y = (y * (Max - min)) + min;
		y1 = x * x;
		if (y <= y1) {
			k ++;
		}
	}
	return k;
}


int main(int argc, char * argv[]) {
	a = 0;
	b = 5;
	min = 0;
	Max = 25;

	int PORT = atoi(argv[1]);

	int sockfd, newsockfd;
	int clilen;
	int n; //число точек, передает клиент
	struct sockaddr_in servaddr, cliaddr;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Can't create listen socket");
		exit(-1);
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("Can't bind listen socket");
		close(sockfd);
		exit(-1);
	}

	if (listen(sockfd, 1) < 0) {
		perror("listen error");
		close(sockfd);
		exit(-1);
	}
	clilen = sizeof(cliaddr);
	if ((newsockfd = accept(sockfd, (struct sockaddr*)&cliaddr, &clilen)) < 0) {
		perror("accept error");
		close(sockfd);
		exit(-1);
	}
	int readbytes = recv(newsockfd, &n, sizeof(int), 0);
	if (readbytes < 0) {
		printf("invalid data");
		close(sockfd);
		close(newsockfd);
		exit(-1);
	}
	int res = calculate(&n);
	int writebytes = send(newsockfd, &res, sizeof(res), 0);
	if (writebytes < 0) {
		perror("Can't send\n");
		close(sockfd);
		close(newsockfd);
		exit(-1);
	}
	close(newsockfd);
	close(sockfd);
	return 0;
}
