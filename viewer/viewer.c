#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <termios.h>

long offset = 0;
int curLine = 0;

int winSize()
{
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_row;
}

void printLines(FILE *input, int row)
{
	long line = 1;
	int c;

	fseek(input, offset, SEEK_SET);

	while ((c = fgetc(input)) != EOF)
	{
		offset++;
		printf("%c", c);
		if (c == '\n')
			line++;
		if (line == row || row == 1)
			break;
	}
	if (row == 1 && line == 2)
		curLine++;
	if (row == 2)
		curLine++;
	if (row > 2)
		curLine += line;
}

void goBack(FILE *input, int lines)
{
	int c;
	int line = 0;
	lines--;
	while ((c = fgetc(input)) != EOF)
	{
		if (c == '\n')
			line++;
		if (offset < 2)
			break;
		fseek(input, -2, SEEK_CUR);
		offset--;
		if (line == (winSize() + lines))
		{
			curLine -= (winSize() + lines);
			break;
		}
	}
}

void printUsage()
{
	printf("Usage:\n");
	printf("Read from file:\t\t./viewer file\n");
	printf("Read from stdin:\t./viewer < file\n");
	printf("\t\t\tcat file | ./viewer\n\n");
	exit(-1);
}

void printHelp()
{
	printf("Usage:\n");
	printf("1 - Page Back\n");
	printf("2 - Page Forward\n");
	printf("3 - Line Back\n");
	printf("4 - Line Forward\n");
	printf("5 - Char Back\n");
	printf("6 - Char Forward\n");
	printf("h - Display Help\n");
	printf("q - Quit\n");
}


int getChar(void)
{
	int ch;
	struct termios oldt, newt;

	tcgetattr (STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO);
	tcsetattr (STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr (STDIN_FILENO, TCSANOW, &oldt);

	return ch;
}

int main(int argc, char const *argv[])
{
	FILE *input;

	int tty = isatty(input);

	if (argc != 2 && tty)
	{
		printUsage();
		return 1;
	}

	if (!tty)
		input = stdin;
	else
	{
		input = fopen(argv[1],"r");
		if (input == NULL)
		{
			fprintf(stderr, "Unable to open '%s': %s\n", argv[1], strerror(errno));
			exit(1);
		}
	}

	printLines(input, winSize());

	while(1)
	{
		char x = getChar();
		if (x == '1')
		{
			goBack(input, winSize());
			printLines(input, winSize());
		}
		if (x == '2')
			printLines(input, winSize());
		if (x == '3')
		{
			goBack(input, 1);
			printLines(input, winSize());
		}
		if (x == '4')
			printLines(input, 2);
		if (x == '5')
		{
			// not working
			goBack(input, 0);
			printLines(input, winSize());
		}
		if (x == '6')
			printLines(input, 1);
		if (x == 'h')
			printHelp();
		if (x == 'q')
			break;
	}
	return 0;
}