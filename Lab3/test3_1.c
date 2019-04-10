#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include <semaphore.h>
int main(int argc,char**argv)
{
	sem_t *sem1;
	sem_t *sem2;
	sem_t *sem3;
	pid_t P1,P2,P3,P4;
	sem1=sem_open("one",O_CREAT,0666,0);
	sem2=sem_open("two",O_CREAT,0666,0);
	sem3=sem_open("three",O_CREAT,0666,0);
	P1=fork();
	if (P1==0)
	{
		printf("I am the process P1\n");
		sem_post(sem1);
	}
	else
	{
		P2=fork();
		if (P2==0)
		{
		sem_wait(sem1);
		printf("I am the process P2\n");
		sem_post(sem1);
		sem_post(sem2);
		}
		else
		{
			P3=fork();
			if (P3==0)
			{
				sem_wait(sem1);
				printf("I am the process P3\n");
				sem_post(sem1);
				sem_post(sem3);
			}
			else
			{
				P4=fork();
				if (P4==0)
				{
					sem_wait(sem2);
					sem_wait(sem3);
					printf("I am the process P4\n");
				}
			}
		}
	}
	sem_close(sem1);
	sem_close(sem2);
	sem_close(sem3);	
	sem_unlink("one");
	sem_unlink("two");
	sem_unlink("three");
	return 0;
}
