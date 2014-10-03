#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define SIZE 1000
#define THREADS 3
#define LOOPS 100

int buffer[SIZE];
int buffer_index;

pthread_mutex_t buffer_mutex;

sem_t full;
sem_t empty;

void put(int value)
{
   if (buffer_index < SIZE)
      buffer[buffer_index++] = value;
   else
      printf("Buffer overflow\n");
}

int get()
{
   if (buffer_index > 0)
      return buffer[--buffer_index];
   else
      printf("Buffer underflow\n");
   return 0;
}

void *producer(void *thread_n)
{
   int thread_number = *(int *)thread_n;
   int value;
   int i = 0;
   while (i++ < LOOPS)
   {
      usleep(rand() % 1000000);
      value = rand() % 100;
      sem_wait(&full);
      pthread_mutex_lock(&buffer_mutex);
      put(value);
      pthread_mutex_unlock(&buffer_mutex);
      sem_post(&empty);
      printf("Producer %d added %d to buffer\n", thread_number+1, value);
   }
   pthread_exit(0);
}

void *consumer(void *thread_n)
{
   int thread_number = *(int *)thread_n;
   int value;
   int i = 0;
   while (i++ < LOOPS)
   {
      usleep(rand() % 1000000);
      sem_wait(&empty);
      pthread_mutex_lock(&buffer_mutex);
      value = get(value);
      pthread_mutex_unlock(&buffer_mutex);
      sem_post(&full);
      printf("Consumer %d removed %d from buffer\n", thread_number+1, value);
   }
   pthread_exit(0);
}

int main()
{
   buffer_index = 0;
   pthread_mutex_init(&buffer_mutex, NULL);
   sem_init(&full,0,SIZE);
   sem_init(&empty,0,0);
   pthread_t thread[THREADS];
   int thread_number[THREADS];
   int i;

   for (i = 0; i < THREADS; i++)
   {
      thread_number[i] = i;
      pthread_create(thread + i,NULL,producer,thread_number + i);
   }

   for (i = 0; i < THREADS; i++)
   {
      thread_number[i] = i;
      pthread_create(thread + i,NULL,consumer,thread_number + i);
   }

   for (i = 0; i < THREADS; i++)
      pthread_join(thread[i], NULL);

   pthread_mutex_destroy(&buffer_mutex);
   sem_destroy(&full);
   sem_destroy(&empty);

   return 0;
}