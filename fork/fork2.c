#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	pid_t child_pid = fork();

	if(child_pid == 0)
	{
		printf("Child1\n");
		sleep(1);
		printf("child1 end\n");
		exit(1);
	}
	printf("Parent\n");
	pid_t child_pid2 = fork();
	if(child_pid2 == 0)
	{
		printf("Child2\n");
		sleep(3);
		printf("child2 end\n");
		exit(-1);
	}
	sleep(2);
	int status;
	pid_t x = wait(&status);
	printf("Child %i finished, Status %i\n", x, status);
	pid_t y = wait(&status);
	printf("Child %i finished, Status: %i\n", y, status);
	printf("parent end\n");
}