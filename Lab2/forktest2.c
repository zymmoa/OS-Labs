#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
int main()
{
int P1pid=getpid();
int P2pid,P3pid,P4pid,P5pid;
printf("\nin P1,pid=%d\n",P1pid);
pid_t P2,P3,P4,P5;
P2=fork();
if(P2<0)printf("error in fork!");
else if(P2==0)
{
P2pid=getpid();
printf("in P2,pid=%d,fatherpid=%d\n",P2pid,getppid());
P4=fork();
if(P4==0)
{
P4pid=getpid();
printf("in P4,pid=%d,fatherpid=%d\n",P4pid,getppid());
}
else{P5=fork();
if(P5==0)
{
P5pid=getpid();
printf("in P5,pid=%d,fatherpid=%d\n",P5pid,getppid());
}}
}
else
{P3=fork();
if(P3<0)printf("error in fork!");
else if(P3==0)
{
P3pid=getpid();
printf("in P3,pid=%d,fatherpid=%d\n",P3pid,getppid());
}}
return 0;
}
