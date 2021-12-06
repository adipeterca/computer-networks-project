#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char * argv[])
{
	if (argc != 3)
		return -1;
	printf("Ok\n");
	int fd = open(argv[1], O_RDWR | O_TRUNC);
	printf("Ok2\n");
	int startvalue = atoi(argv[2]);
	write(fd, &startvalue, sizeof(startvalue));
	printf("Ok3\n");
	startvalue = -2;
	lseek(fd, SEEK_SET, 0);
	int bytesRead = read(fd, &startvalue, sizeof(startvalue));
	printf("bytesRead = %d\n", bytesRead);
	printf("startValue read = %d\n", startvalue);
	return 0;
}
