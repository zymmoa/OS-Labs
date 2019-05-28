#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h> 
#include <windows.h> 
double get_random_r()
{
	//srand( time( 0 ) ); 
	int x ;
	x = rand () ;
	printf("x:%d\n",x);
	double r = x / (RAND_MAX+1.0);
	return r;
}
//确定虚拟内存的尺寸P，工作面的起始位置p，工作面中包含的页数e，工作面移动率m，以及一个范围在謰和謱之间的值t
void Get_RS(int *RS,int P,int p,int e,int m,float t)
{
	int i=0;
	int j=0;
	int real_num = 0 ;
	while( real_num < P )
	{
		for( i = 0 ; i < m && real_num < P; i ++, real_num++ )
		{
			RS [ real_num ] = rand()%(p+e+1)+p;
		}
		if( real_num >= P)
		{
			break;
		}
		double r = get_random_r();
		printf("r:%f\n",r);
		if(r<t)
		{
			p = rand() % P;
		}
		else
		{
			p = (p+1)%P;
		}
		for(j=0;j<10;j++)
		{
			Sleep(60);
		}
	}
}
int OPT(int *RS,int R, int *M,int F)
{
	int P=0;	//缺页中断的次数
	int m,m1;
	int r,r1;
	int flag,flag2;
	int fm,rmax;
	int num=0;
	for(r=0;r<R;r++)	
	{
		flag=0;
		for(m=0;m<num;m++)
		{
			if(M[m]==RS[r])
			{
				flag=1;
				break;
			}
		}
		//如果缺页的话
		if( num <F && flag==0)
		{
			M[num]=RS[r];
			num ++;
			continue;
		}
		if( flag==0 && num==F)
		{
			P++;
			rmax = 0;
			fm = 0;
			for( m1 = 0 ; m1 < F ; m1++ )
			{
				flag2 = 0;
				for( r1 = r ; r1 < R ; r1++ )
				{
					if( RS[r1] == M[m1])
					{
						break;
					}
				}
				if( r1 > rmax )
				{
					rmax = r1;
					fm = m1;
				}
			}
			//printf("fm:%d\n",fm);
			M[fm]=RS[r];
		}
	}
	return P;
	
}

int Random(int *RS, int R, int *M,int F)
{
	int P=0;	//缺页中断的次数
	int m,r;
	int flag;
	for(r=0;r<R;r++)	
	{
		flag=0;
		for(m=0;m<F;m++)
		{
			if(M[m]==RS[r])
			{
				flag=1;
				break;
			}
		}
		if(!flag)
		{
			P++;
			int Page = rand()%F;
			M[Page] = RS[r];
		}
	}
	return P;
	
}

int FIFO(int *RS, int R, int *M,int F)
{
	int P=0;	//缺页中断的次数
	int m;
	int r;
	int flag;
	int pos;
	for(m=0;m<F;m++)
	{
		M[m] = RS[m];
	}
	pos = 0;
	for(r=0;r<R;r++)	
	{
		flag=0;
		for(m=0;m<F;m++)
		{
			if(M[m]==RS[r])
			{
				flag=1;
				break;
			}
		}
		if( !flag )
		{
			P++;
			M[pos] = RS[r];
			pos ++;
			pos = pos%F;
		}
	}
	return P;
	
}

int LRU(int *RS, int R, int *M,int F)
{
	int P = 0 ;	//缺页中断的次数
	int m , r , i , flag ;
	int num = 0;
	int *temp;
	temp = ( int * ) malloc ( sizeof ( int ) * F ) ; 
	for( r = 0 ; r < R ; r ++)	
	{
		flag = 0 ;
		for( m = 0 ; m < num ; m ++ )
		{
			if( RS [ r ] == M [ m ] )
			{
				flag = 1 ;
				break;
			}	
		}

		if( flag == 1 )
		{

			for ( i = 0 ; i < num ; i ++)
			{
				temp [ i ] ++ ;
			}

			temp [ m ] = 0 ;

		}

		if( num == F && flag == 0)
		{

			P ++ ;
			int max = -1 , max_i;
			for ( i = 0 ; i < F; i ++ )
			{
				if ( temp [ i ] > max)
				{
					max_i = i;
					max = temp [ i ] ;
				}
				temp [ i ] ++ ;
			}

			M [ max_i ] = RS [ r ] ;
			temp [ max_i ] = 0 ;

		}
		
		if( num < F && flag == 0 )
		{
			for ( i = 0 ; i < num ; i ++)
			{
				temp [ i ] ++ ;
			}

			M [ num ] = RS [ r ];
			temp [ num ] = 0 ;
			num ++ ;
		}

	}
	return P;	
}


int Clock(int *RS, int R, int *M,int F)
{
	int P=0;	//缺页中断的次数
	int m,r,flag,pos,num;
	int *temp;
	int i;
	temp=(int*)malloc(sizeof(int)*F);
	num = 0 ;
	for(r=0;r<R;r++)	
	{
		flag=0;

		for(m=0;m<num;m++)
		{
			if(M[m]==RS[r])
			{
				flag=1;
				break;
			}
		}

		if ( flag == 0 && num == F )
		{
			P++ ;
			pos = 0;

			for( i = 0 ; i < F ; i ++ )
			{

				if (temp[i]==1)
				{
					temp[i]=0;
				}
				else
				{
					pos = i ;
					break ; 
				}

			}

			M [ pos ] = RS [ i ];
			temp [ pos ] = 1;
			
		}
		if ( flag == 0 && num < F )
		{
			M [ num ] = RS [ r ] ;
			temp [ num ] = 1 ;
			num ++ ;
		}

	}
	return P;
}

int main()
{
	int *M;
	int F=3;
	M=(int*)malloc(sizeof(int)*F);
	/*int *RS;
	int P,p,e,m;
	float t;
	P = 100 ;
	p = 5 ;
	e = 12 ;
	m = 4 ;
	t = 0.81 ;
	RS = ( int * ) malloc ( sizeof ( int ) * P ) ;
	
	Get_RS(RS,P,p,e,m,t);
	for( int i = 0 ; i < P ; i ++ )
	{
		printf("%d ",RS[i]);
	}
	printf("\n");*/
	int RS[12] = {7,3,5,2,10,8,5,8,9,9,7,9};
	int P = 12;
	printf("%d\n",OPT(RS,P,M,F));
	printf("%d\n",Random(RS,P,M,F));
	printf("%d\n",FIFO(RS,P,M,F));
	printf("%d\n",LRU(RS,P,M,F));
	printf("%d\n",Clock(RS,P,M,F));
	return 0;
}