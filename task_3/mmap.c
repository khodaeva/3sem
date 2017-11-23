#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>



int main(int argc, char *argv[]) {
	clock_t start, stop;
	start = clock();
	char * source = argv[argc - 2];
	char * dest = argv[argc - 1];
	int fd_source, fd_dest, i;
	size_t length;
	struct stat buf;
	if ((fd_source = open(source, O_RDWR)) < 0) {
		printf("Can't open source file\n");
		exit(-1);
	}
	if ((fd_dest = open(dest, O_RDWR|O_CREAT, 0666)) < 0) {
		printf("Can't open destination file\n");
		exit(-1);
	}


	if ((i = fstat(fd_source, &buf)) < 0) {
		printf("Can't find source file length\n");
		exit(-1);
	}
	length = buf.st_size;

	if ((i = ftruncate(fd_dest, length)) < 0) {
		printf("Can't change destination file size\n");
		exit(-1);
	}
	

	void * ptr_source;
	void * ptr_dest;
	if ((ptr_source = mmap (NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_source, 0)) == MAP_FAILED) {
		printf("Source file mmap error\n");
		exit(-1);
	}
	if ((ptr_dest = mmap (NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dest, 0)) == MAP_FAILED) {
		printf("Destination file mmap error\n");
		exit(-1);
	}


	if ((i = close(fd_source)) < 0) {
		printf("Can't close source file\n");
		exit(-1);
	}
	if ((i = close(fd_dest)) < 0) {
		printf("Can't close destination file\n");
		exit(-1);
	}


	memcpy (ptr_dest, ptr_source, length);


	munmap (ptr_dest, length);
	munmap (ptr_source, length);

	stop  = clock();
	//printf("%ld\n", stop-start);
	return 0;
}
