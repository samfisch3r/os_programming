#include <stdio.h>
#include <unistd.h>

static char* PIPE = "/tmp/test_pipe";
int main()
{
	pid_t child_pid = fork();
	char sign;

	if(child_pid == 0)
	{
		FILE* input = fopen(PIPE, "r");

		while((sign = fgetc(input) ) != EOF)
			printf("%c",sign);
		fclose(input);
		printf("child end\n");
	}
	else
	{
		FILE* input = fopen(PIPE, "a");

		fputs("TEST 1 2 3\n", input);
		fclose(input);
		printf("parent end\n");
	}
}