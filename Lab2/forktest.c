#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
int main()
{
pid_t fpid;
fpid=fork();
if(fpid<0)printf("error in fork!");
else if(fpid==0)
{
execl("/usr/bin/vi","vi","/home/zym/lll.txt",NULL);
}
else{
for( int i=0;;i++){}
}
return 0;
}
