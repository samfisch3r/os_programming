#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 50
#define STRUCT_SIZE	100000

struct pair_count {
	short pair;
	int count;
};

pthread_mutex_t mutex;

struct pair_count pc[STRUCT_SIZE];
int pair_num = 0;

int find_pair(short* pair)
{
	int i;
	for (i = 0; i < pair_num; i++)
	{
		if (pc[i].pair == pair)
			return i;
	}
	return -1;
}

void add_pair(short* pair)
{
	pthread_mutex_lock (&mutex);
	int ret = find_pair(pair);
	if (ret != -1)
		(pc[ret].count)++;
	else
	{
		pc[pair_num].pair = pair;
		(pc[pair_num].count)++;
		pair_num++;
	}
	pthread_mutex_unlock (&mutex);
}

void read_pairs()
{
	int i;
	for (i = 0; i < pair_num; i++)
	{
		printf("Short: %hd\n", pc[i].pair);
		printf("Count: %i\n", pc[i].count);
	}
}

void *file_read(void *filename)
{
	FILE *fp;
	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		printf("ERROR, while opening the file %s\n", filename);
		pthread_exit(NULL);
	}

	short x;
	while (fread(&x, sizeof x, 1, fp) != 0)
		add_pair(x);

	pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
	int i;
	int rc;
	pthread_t thread[NUM_THREADS];
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_mutex_init(&mutex, NULL);

	for (i = 1; i < argc; ++i)
	{
		rc = pthread_create(&thread[i-1], &attr, file_read, (void *)argv[i]);
		if (rc)
		{
			printf("ERROR, return code from pthread_create() is %d\n", rc);
			exit (-1);
		}
	}

	pthread_attr_destroy(&attr);

	for (i = 1; i < argc; ++i)
	{
		rc = pthread_join(thread[i-1], NULL);
		if (rc)
		{
			printf("ERROR, return code from pthread_join() is %d\n", rc);
			exit(-1);
		}
	}

	read_pairs();

	pthread_mutex_destroy(&mutex);
	pthread_exit(NULL);

	return 0;
}