#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <pthread.h>
#include<fcntl.h>
#include<sys/stat.h>
#include <semaphore.h>
int ticketCount=1000;
int temp=0;
void *sale(void *arg)
{
	for(;;)
	{
//		sem_wait(empty);
//		sem_wait(mutex);
		temp=ticketCount;
		pthread_yield();
		temp=temp-1;
		pthread_yield();
		ticketCount=temp;
		printf("卖出一张票，现在剩余%d张票\n",ticketCount);
		//sleep(0.5); 
//		sem_post(mutex);
//		sem_post(full);
	}
	return NULL;
}

void *refund(void *arg)
{
	for(;;)
	{
//		sem_wait(full);
//		sem_wait(mutex);
		temp=ticketCount;
		pthread_yield();
		temp=temp+1;
		pthread_yield();
		ticketCount=temp;
		printf("退回一张票，现在剩余%d张票\n",ticketCount);
		//sleep(0.5); 
//		sem_post(mutex);
//		sem_post(empty);
	}
	return NULL;
}
int main(int argc,char**argv)
{
	sem_t *mutex;
	sem_t *empty;
	sem_t *full;
//	mutex=sem_open("one",O_CREAT,0666,1);
//	empty=sem_open("two",O_CREAT,0666,1000);
//	full=sem_open("three",O_CREAT,0666,0);
	pthread_t p1, p2;
	pthread_create(&p1, NULL, sale, NULL);
	pthread_create(&p2, NULL, refund, NULL);
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
//	sem_close(mutex);
//	sem_close(empty);
//	sem_close(full);	
//	sem_unlink("one");
//	sem_unlink("two");
//	sem_unlink("three");
	return 0;
}
