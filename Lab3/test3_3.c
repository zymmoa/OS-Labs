#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include <semaphore.h>
#include <pthread.h>
//sem_t *mutex;
//sem_t *empty;
//sem_t *full;
char buf[10]={0};
void *readd(void *arg)
{
	int in=0;int a=67;
	for(;;)
	{
		//sem_wait(empty);
		//sem_wait(mutex);
		//scanf("%c",&buf[in]);
		buf[in]=a;
		in=(in+1)%10;
		a+=1;
		sleep(1);
		//sem_post(mutex);
		//sem_post(full);
	}
	return NULL;
}

void *writte(void *arg)
{
	int out=0;
	for(;;)
	{
		//sem_wait(full);
		//sem_wait(mutex);
		printf("%c\n",buf[out]);
		out=(out+1)%10;
		sleep(1); 
		//sem_post(mutex);
		//sem_post(empty);
	}
	return NULL;
}
int main(int argc,char**argv)
{
	//mutex=sem_open("one",O_CREAT,0666,1);
	//empty=sem_open("two",O_CREAT,0666,10);
	//full=sem_open("three",O_CREAT,0666,0);
	pthread_t p1, p2;
	pthread_create(&p1, NULL, readd, NULL);
	pthread_create(&p2, NULL, writte, NULL);
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
	//sem_close(mutex);
	//sem_close(empty);
	//sem_close(full);	
	//sem_unlink("one");
	//sem_unlink("two");
	//sem_unlink("three");
	return 0;
}

