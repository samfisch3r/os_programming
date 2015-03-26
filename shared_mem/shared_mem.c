#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <stdlib.h>
#include <getopt.h>

#define SEM_ID	250
#define SHM_SIZE	100000

struct pair_count {
	short pair;
	int count;
};

void sem_lock(int sem_set_id)
{
	struct sembuf sem_op;
	sem_op.sem_num = 0;
	sem_op.sem_op = -1;
	sem_op.sem_flg = 0;
	semop(sem_set_id, &sem_op, 1);
}

void sem_unlock(int sem_set_id)
{
	struct sembuf sem_op;
	sem_op.sem_num = 0;
	sem_op.sem_op = 1;
	sem_op.sem_flg = 0;
	semop(sem_set_id, &sem_op, 1);
}

int find_pair(int sem_set_id, int* pair_num, struct pair_count* pc, short* pair)
{
	int i;
	for (i = 0; i < (*pair_num); i++)
	{
		if (pc[i].pair == pair)
			return i;
	}
	return -1;
}

void add_pair(int sem_set_id, int* pair_num, struct pair_count* pc, short* pair)
{
	sem_lock(sem_set_id);
	int ret = find_pair(sem_set_id, pair_num, pc, pair);
	if (ret != -1)
		(pc[ret].count)++;
	else
	{
		pc[*pair_num].pair = pair;
		(pc[*pair_num].count)++;
		(*pair_num)++;
	}
	sem_unlock(sem_set_id);
}

void read_pairs(int sem_set_id, int* pair_num, struct pair_count* pc)
{
	int i;
	sem_lock(sem_set_id);
	for (i = 0; i < (*pair_num); i++)
	{
		printf("Short: %hd\n", pc[i].pair);
		printf("Count: %i\n", pc[i].count);
	}
	sem_unlock(sem_set_id);
}

int process_options(int argc, char *argv[])
{
	int val = 0;
	int opt = 0;
	while((opt = getopt(argc, argv, "sc")) != -1)
	{
		switch (opt)
		{
		case 's':
			val = 1;
			break;
		case 'c':
			val = 2;
			break;
		default:
			val = 0;
			break;
		}
	}
	return val;
}

int delete_segment(int seg_id)
{
    if ((shmctl(seg_id,IPC_RMID,0))==-1)
    	return -1;
    else
        return 0;
}

void clean_segments()
{
	int i;
    struct shmid_ds shm_info;
    struct shmid_ds shm_segment;
    int max_id = shmctl(0,SHM_INFO,&shm_info);
    if (max_id>=0)
    {
        for (i=0;i<=max_id;++i)
        {
                int shm_id = shmctl(i , SHM_STAT , &shm_segment);
                if (shm_id<=0)
                    continue;
                else if (shm_segment.shm_nattch==0)
                    delete_segment(shm_id);
        }
    }
}

int main(int argc, char const *argv[])
{
	int val = process_options(argc, argv);

	int sem_set_id;
	int shm_id;
	char* shm_addr;
	int* pair_num;
	struct pair_count* pc;
	int rc;

	if (val == 2)
	{
		clean_segments();
		exit(0);
	}

	if (val == 1)
	{
		sem_set_id = semget(SEM_ID, 1, IPC_CREAT | 0600);
		if (sem_set_id == -1)
		{
			perror("main: semget");
			exit(1);
		}

		shm_id = shmget(100, SHM_SIZE, IPC_CREAT | IPC_EXCL | 0600);
		if (shm_id == -1)
		{
			perror("main:shmget: ");
			exit(1);
		}
		exit(0);
	}

	sem_set_id = semget(SEM_ID, 1, 0600);
	if (sem_set_id == -1)
	{
		perror("main: semget");
		exit(1);
	}

	shm_id = shmget(100, SHM_SIZE, IPC_EXCL | 0600);
	if (shm_id == -1)
	{
		perror("main:shmget: ");
		exit(1);
	}

	shm_addr = shmat(shm_id, NULL, 0);
	if (!shm_addr)
	{
		perror("main: shmat: ");
		exit(1);
	}

	pair_num = (int*) shm_addr;
	*pair_num = 0;
	pc = (struct pair_count*) ((void*)shm_addr+sizeof(int));

	FILE *fp;
	int tty = isatty(fp);
	if (!tty)
		fp = stdin;

	short x;
	while (fread(&x, sizeof x, 1, fp) != 0)
		add_pair(sem_set_id, pair_num, pc, x);

	read_pairs(sem_set_id, pair_num, pc);

	return 0;
}