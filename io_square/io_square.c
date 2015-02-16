#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main ()
{
	int fd;
	mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
	fd = open("/tmp/test", O_RDWR | O_CREAT | O_TRUNC, mode);
	if (fd == -1)
	{
		printf("Unable to open /tmp/test\n");
		return 1;
	}

	int i;
	for (i = 99; i > 0; --i)
	{
		char ch[6];
		sprintf(ch, "%i\n", i*i);
		lseek(fd, 6*i, SEEK_SET);
		write(fd, (void *) ch, 6);
	}

	close(fd);
}