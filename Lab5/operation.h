#pragma once
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 


//辅助函数
int show_openlist();    //显示打开文件表，返回打开文件个数
void directory();     //显示目录文件详细信息
void show_help();     //该文件系统的帮助
void show_ldisk();     //显示磁盘内容辅助用
					   //核心函数
void read_block(int, char *);  //文件系统与IO设备的接口函数读取块 
void write_block(int, char *);  //文件系统与IO设备的接口函数写入块
void Init();      //初始化文件系统
int create(char *);     //创建文件 
int destroy(char *);    //删除文件 
int open(char *);     //打开文件
int close(int);      //关闭文件
int read(int, int, int);    //读文件
int write(int, int, int);    //写文件 
int write_buffer(int, int);   //把缓冲区内容写入文件 
int lseek(int, int);     //定位文件指针 
void Init_block(char, int);   //初始化字符数组块辅助

void read_block(int i, char *p) //读磁盘块该函数把逻辑块的内容读入到指针p指向的内存位置拷贝的字符个数为存储块的长度 
{
	char * temp = (char *)malloc(sizeof(char));
	temp = p;
	for (int a = 0; a < B;)
	{
		*temp = ldisk[i][a];
		a++;
		temp++;
	}
}

void write_block(int i, char *p) //写磁盘块该函数把指针指向的内容写入逻辑块拷贝的字符个数为存储块的长度
{
	char * temp = (char *)malloc(sizeof(char));
	temp = p;
	for (int a = 0; a < B;)
	{
		ldisk[i][a] = *temp;
		a++;
		temp++;
	}
}

void Init_block(char *temp, int length)
{
	int i;
	for (i = 0; i < length; i++)
	{
		temp[i] = '\0';
	}
}
int write_buffer(int index, int list)
{
	int i;
	int j;
	int freed;
	char temp[B];

	int buffer_length = BUFFER_LENGTH;
	for (i = 0; i < BUFFER_LENGTH; i++)
	{
		if (open_list[list].buffer[i] == '\0')
		{
			buffer_length = i;     //缓冲区有效长度 
			break;
		}
	}

	int x = open_list[list].pointer[0];
	int y = open_list[list].pointer[1];
	int z = B - y;         //当前块空闲容量 
						   // printf("W:buffer_length: %d  x:%d y:%d z:%d\n",buffer_length,x,y,z); 

	if (buffer_length < z)     //块容量可写入缓冲区不需要再分配 
	{
		read_block(x, temp);
		strncat(temp + y, open_list[list].buffer, buffer_length);   //缓冲区接入
																	//printf("temp[0]: %c\n",temp[0]); 
		write_block(x, temp);

		read_block(index + FILE_SIGN_AREA, temp); //更¨1改?文?件t长?è度¨¨ 
												  //printf("temp[1] = %d\n",temp[1]); 
		temp[1] += buffer_length;
		//printf("temp[1] = %d\n",temp[1]); 
		write_block(index + FILE_SIGN_AREA, temp);

		open_list[list].pointer[0] = x;
		open_list[list].pointer[1] = y + buffer_length;     //更新文件读写指针 
	}
	else         //大于需要分配新块 
	{
		read_block(index + FILE_SIGN_AREA, temp);
		if (temp[2] + (buffer_length - z) / B + 1 > FILE_BLOCK_LENGTH)
		{
			printf("文件分配数组不够分配?\n");
			return ERROR;
		}

		//分别三部分 
		//第一部分填满
		read_block(x, temp);
		strncat(temp + y, open_list[list].buffer, z);   //缓冲区接入的长度填满当前块 
		write_block(x, temp);
		//第二部分还需要分配(buffer_length - z)/B+1块

		//寻找文件区目录项之后的空闲块分配新块
		for (i = 0; i < (buffer_length - z) / B; i++)
		{
			for (j = K + FILE_NUM; j < L; j++)
			{
				read_block((j - K) / B, temp);
				if (temp[(j - K) % B] == FREE)
				{
					freed = j;
					//   printf("freed= : %d\n",freed); 
					break;
				}
			}
			if (j == L)
			{
				printf("磁盘已满分配失败\n");
				return ERROR;
			}

			Init_block(temp, B);
			strncpy(temp, (open_list[list].buffer + z + (i*B)), B);
			write_block(freed, temp);    //写入内容 

			read_block((freed - K) / B, temp);   //更改位图状态 
			temp[(freed - K) % B] = BUSY;
			write_block((freed - K) / B, temp);   //写入磁盘位图(文件内容所对应的) 
			read_block(index + FILE_SIGN_AREA, temp);
			temp[2] ++;        //增加文件分配数组长度 
			temp[2 + temp[2]] = freed;
			write_block(index + FILE_SIGN_AREA, temp);
		}
		//(buffer_length - z)%B 不满块在尾部的部分
		for (j = K + FILE_NUM; j < L; j++)
		{
			read_block((j - K) / B, temp);
			if (temp[(j - K) % B] == FREE)
			{
				freed = j;
				break;
			}
		}
		if (j == L)
		{
			printf("磁盘已满分配失败\n");
			return ERROR;
		}
		Init_block(temp, B);
		// printf("(buffer_length - z)%B = %d\n",(buffer_length - z)%B); 
		strncpy(temp, (open_list[list].buffer + z + (i*B)), (buffer_length - z) % B);
		write_block(freed, temp);    //写入内容 
		read_block((freed - K) / B, temp);   //更改位图状态
		temp[(freed - K) % B] = BUSY;
		write_block((freed - K) / B, temp);   //写入磁盘位图(文件内容所对应的) 
		read_block(index + FILE_SIGN_AREA, temp);
		temp[2] ++;        //增加文件分配数组长度 
		temp[2 + temp[2]] = freed;
		write_block(index + FILE_SIGN_AREA, temp);
		read_block(index + FILE_SIGN_AREA, temp);
		temp[1] += buffer_length;    //更改文件长度 
		write_block(index + FILE_SIGN_AREA, temp);

		open_list[list].pointer[0] = freed;
		open_list[list].pointer[1] = (buffer_length - z) % B;
	}
	// printf("X = %d, Y = %d\n",open_list[list].pointer[0],open_list[list].pointer[1]); 
}
int lseek(int index, int pos)
/*
把文件的读写指针移动到pos指定的位置。
pos是一个整数表示从文件开始位置的偏移量。
文件打开时读写指针自动为0。
每次读写操作之后它指向最后被访问的字节的下一个位置。
lseek 能够在不进行读写操作的情况下改变读写指针位置。
*/
{
	int i;
	int list = -1;
	char temp[B];
	int pos_i = pos / B;    //在文件所有块数中的第X块
	int pos_j = pos % B;    //在第X块中D的X个位置 
							//根据Yindex找表目
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].filesignnum == index)
		{
			list = i;
			break;
		}
	}

	if (list == -1)     //没找到
	{
		printf("没找到当前索引号文件,操作失败\n");
		return ERROR;
	}
	if (open_list[list].flag != BUSY)  //输入的index对应文件没被打开
	{
		printf("输入的索引号有误,操作失败\n");
		return ERROR;
	}
	//根据Y表目中的index即文件描述符序找到具体内容
	read_block(open_list[list].filesignnum + FILE_SIGN_AREA, temp);
	if (pos_i > temp[2] - 1)           //大于文件实际的块数时
	{
		printf("异常越界，定位失败\n");
		return ERROR;
	}

	//定位指针
	open_list[list].pointer[0] = temp[3 + pos_i];    //该文件的第X块的实际磁盘地址 
	open_list[list].pointer[1] = pos_j;

	return OK;

}


void Init()
/*初始化磁盘将磁盘全部置空然后创建0号文件描述符为根目录的文件描述符初始化位图区*/
{
	int i;
	char temp[B];


	for (i = 0; i < L; i++)
	{
		Init_block(temp, B);
		write_block(i, temp);   //初始化磁盘
	}

	for (i = K; i < L; i++)      //初始化保留区中的位图
	{
		read_block((i - K) / B, temp);
		temp[(i - K) % B] = FREE;
		write_block((i - K) % B, temp);
	}
	//******初始化目录的文件标识符
	filesign temp_cnt_sign;
	temp_cnt_sign.filesign_flag = 1;
	temp_cnt_sign.file_length = 0;     //文件长度初始为0   
	temp_cnt_sign.file_block = FILE_BLOCK_LENGTH; //分配FILE_BLOCK_LENGTH*B的空间在数据区前段用于储存目录项

	Init_block(temp, B);
	temp[0] = temp_cnt_sign.filesign_flag;
	temp[1] = temp_cnt_sign.file_length;
	temp[2] = temp_cnt_sign.file_block;

	for (i = 0; i < FILE_BLOCK_LENGTH; i++)
	{
		temp[i + 3] = K + i;         //默认数据区的前FILE_BLOCK_LENGTH被根目录占用
	}
	write_block(FILE_SIGN_AREA, temp);
	//**************更改位图区状态
	read_block(0, temp);
	for (i = 0; i < FILE_NUM; i++)
	{
		temp[i] = FREE;      //此时未创建文件
	}
	write_block(0, temp);
}


int create(char filename[])
/**************************根据文件名创建文件
* 找一空闲文件描述符
* 在文件目录里为新创建的文件分配一个目录项可能需要为目录文件分配新的磁盘块
* 在分配到的目录项里记录文件名以及描述符编号
* 返回状态信息
***************************/
{
	int i;
	int frees;       //空闲的文件描述符位置  
	int freed;       //空闲数据区位置目录项 
	int freed2;
	char temps[B];
	char tempc[B];
	char temp[B];
	//***************查看文件名是否存在
	for (i = K; i < K + FILE_NUM; i++)
	{
		read_block((i - K) / B, temp);
		if (temp[(i - K) % B] == BUSY)
		{
			read_block(i, temp);
			if (strncmp(temp + 1, filename, FILE_NAME_LENGTH) == 0)
			{
				printf("该目录已经存在文件名为%s的文件\n", filename);
				return ERROR;
			}
		}
	}
	//***************寻找保留区中空闲的文件描述符
	for (i = FILE_SIGN_AREA; i < K; i++)
	{
		read_block(i, temp);
		if (temp[0] == FREE)
		{
			frees = i;
			break;
		}
	}
	if (i == K)
	{
		printf("没有空闲的文件描述符\n");
		return ERROR;
	}
	//****************寻找数据区目录描述符所指定的中空闲的存储块
	for (i = K; i < K + FILE_NUM; i++)
	{
		read_block((i - K) / B, temp);
		if (temp[(i - K) % B] == FREE)
		{
			freed = i;
			break;
		}
	}
	if (i == K + FILE_NUM)
	{
		printf("文件个数已达上限\n");
		return ERROR;
	}
	//******************寻找文件区目录项之后的空闲块分配给新创建的文件 
	for (i = K + FILE_NUM; i < L; i++)
	{
		read_block((i - K) / B, temp);
		if (temp[(i - K) % B] == FREE)
		{
			freed2 = i;
			break;
		}
	}
	if (i == L)
	{
		printf("磁盘已满分配失败\n");
		return ERROR;
	}
	//*****************无问题后开始操作 
	filesign temp_filesign;      //创建临时文件描述符 
	contents temp_contents;      //创建临时目录项

								 //**************构建文件描述符
	temp_filesign.filesign_flag = 1;
	temp_filesign.file_length = 0;
	temp_filesign.file_block = 1;


	Init_block(temps, B);
	temps[0] = temp_filesign.filesign_flag;
	temps[1] = temp_filesign.file_length;
	temps[2] = temp_filesign.file_block;
	temps[3] = freed2;
	for (i = 4; i < FILE_BLOCK_LENGTH; i++)
	{
		temps[i] = '\0';
	}
	write_block(frees, temps);    //写入磁盘文件描述符区
								  //*****************构建目录项插入目录文件描述符所指定的数组块
	temp_contents.filesignnum = frees - FILE_SIGN_AREA;
	strncpy(temp_contents.filename, filename, FILE_NAME_LENGTH);

	Init_block(tempc, B);
	tempc[0] = temp_contents.filesignnum;
	tempc[1] = '\0';
	strcat(tempc, temp_contents.filename);
	write_block(freed, tempc);    //写入磁盘数据区
								  //*****************更改位图状态
	read_block((freed - K) / B, temp);   //更改位图状态(目录项所对应的) 
	temp[(freed - K) % B] = BUSY;
	write_block((freed - K) / B, temp);   //写入磁盘位图 

	read_block((freed2 - K) / B, temp);   //更改位图状态
	temp[(freed2 - K) % B] = BUSY;
	write_block((freed2 - K) / B, temp);   //写入磁盘位图(文件内容所对应的) 

										   //****************增加目录文件描述符中的长度项 
	read_block(FILE_SIGN_AREA, temp);
	temp[1]++;
	write_block(FILE_SIGN_AREA, temp);

	return OK;

}

int destroy(char * filename)
/**************************删除指定文件
* 在目录里搜索该文件的描述符编号
* 删除该文件对应的目录项并更新位图
* 释放文件描述符
* 返回状态信息
***************************/
{
	int i;
	int dtys;          //将要删除y的文件的目录项的文件描述符位置
	int dtyd;          //将要删除y的文件的目录项位置
	int use_block;         //该文件实际使用的块数
	int index;
	char temp[B];
	char tempd[B];
	//***************依据文件名寻找文件的目录项和文件描述符
	for (i = K; i < K + FILE_NUM; i++)
	{
		read_block((i - K) / B, temp);
		if (temp[(i - K) % B] == BUSY)
		{
			read_block(i, temp);
			if (strncmp(temp + 1, filename, FILE_NAME_LENGTH) == 0)
			{
				dtyd = i;        //找到文件目录项位置
				dtys = temp[0] + FILE_SIGN_AREA;  //找到文件目录项对应的文件描述符位置(序号为atemp[0]) 
				index = temp[0];
				break;
			}
		}
	}
	if (i == K + FILE_NUM)
	{
		printf("没有找到该文件t\n");
		return ERROR;
	}
	//************根据文件描述符即index查看该文件是否打开
	int list = -1;
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].filesignnum == index)
		{
			list = i;
			break;
		}
	}
	if (open_list[list].flag == BUSY && list != -1)
	{
		printf("该文件已经被打开,需要关闭才能删除\n");
		return ERROR;
	}
	//****************找到位置后开始操作 
	//****************依次更新文件块数组中指定的块位图区 
	read_block(dtys, temp);
	use_block = temp[2];
	for (i = 0; i < use_block; i++)
	{
		read_block((temp[i + 3] - K) / B, tempd);
		tempd[(temp[i + 3] - K) % B] = FREE;
		write_block((temp[i + 3] - K) / B, tempd);
	}
	//***************删除该目录项
	Init_block(temp, B);
	write_block(dtys, temp);
	//***************删除文件描述符
	Init_block(temp, B);
	write_block(dtyd, temp);

	//****************更改位图区 
	read_block((dtyd - K) / B, temp);
	temp[(dtyd - K) % B] = FREE;
	write_block((dtyd - K) / B, temp);
	//**************目录文件描述符中的长度减1 
	read_block(FILE_SIGN_AREA, temp);
	temp[1]--;
	write_block(FILE_SIGN_AREA, temp);
	return OK;
}

int open(char * filename)
/***************************打开文件
* 该函数返回的索引号可用于后续的read, write, lseek, 或close操作。
* 搜索目录找到文件对应的描述符序号
* 在打开文件表中分配一个表目
* 在分配到的表目中把读写指针置为0并记录描述符编号
* 读入文件的第一块到读写缓冲区中
* 返回分配到的表目在打开文件表中的索引号
***************************/
{
	int i;
	int opd;
	int ops;
	int list;
	char temp[B];
	int index;
	//***************依据文件名寻找文件的目录项和文件描述符
	for (i = K; i < K + FILE_NUM; i++)
	{
		read_block((i - K) / B, temp);
		if (temp[(i - K) % B] == BUSY)
		{
			read_block(i, temp);
			if (strncmp(temp + 1, filename, FILE_NAME_LENGTH) == 0)
			{
				opd = i;        //找到文件目录项位置 
				ops = temp[0];       //找到文件目录项对应的文件描述符序号
									 // printf("opd: %d,ops: %d\n",opd,ops); 
				break;
			}
		}
	}
	if (i == K + FILE_NUM)
	{
		printf("没有找到该文件\n");
		return ERROR;
	}

	//*************查看该文件是否被打开 
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].filesignnum == ops && open_list[i].flag == BUSY)
		{
			printf("该文件已经被打开\n");
			return ERROR;
		}
	}

	//**************找一块没使用的表目以便分配
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].flag != BUSY)
		{
			list = i;
			break;
		}
	}

	//******************对表目进行操作

	open_list[list].filesignnum = ops;        //写入文件描述符序号 

	open_list[list].flag = BUSY;         //置标志位为占用

	index = open_list[list].filesignnum;       //生成索引号(相当于该文件目录项的文件描述符序号)   
	lseek(index, 0);             //指针指向文件首部(即指针指为0) 

	Init_block(open_list[list].buffer, BUFFER_LENGTH);    //初始化缓冲区
	read_block(open_list[list].pointer[0], temp);     //读文件的首块
	strncpy(open_list[list].buffer, temp, BUFFER_LENGTH);    //把文件的第一块写入缓冲区 
	return OK;
}

int close(int index)
/***************************关闭文件
* 把缓冲区的内容写入磁盘
* 释放该文件再打开文件表中对应的表目
* 返回状态信息
* ***************************/
{
	int i;
	int list = -1;
	char temp[B];
	//***************根据Yindex找表目 
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].filesignnum == index)
		{
			list = i;
			break;
		}
	}
	if (list == -1)     //没找到 
	{
		printf("没找到当前索引号文件，操作失败\n");
		return ERROR;
	}
	if (open_list[list].flag != BUSY)  //输入的index对应文件没被打开
	{
		printf("输入的索引号有误，操作失败\n");
		return ERROR;
	}

	write_buffer(index, list);
	Init_block(open_list[list].buffer, BUFFER_LENGTH);
	open_list[list].filesignnum = FREE;
	open_list[list].flag = FREE;
	open_list[list].pointer[0] = NULL;
	open_list[list].pointer[1] = NULL;
	return OK;
}


int read(int index, int mem_area, int count)
/*
从指定文件顺序读入count个字节mem_area指定的内存位置。
读操作从文件的读写指针指示的位置开始
*/
{
	int i;
	int list = -1;
	char temp[B];
	//***************根据index找打开文件表表目
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].filesignnum == index)
		{
			list = i;
			break;
		}
	}
	if (list == -1)     //没找到
	{
		printf("没找到当前索引号文件，操作失败\n");
		return ERROR;
	}
	if (open_list[list].flag != BUSY)  //输入的index对应文件没被打开
	{
		printf("输入的索引号有误，操作失败\n");
		return ERROR;
	}
	//***************根据index找文件描述符 
	char temp_output[OUTPUT_LENGTH];
	Init_block(temp_output, OUTPUT_LENGTH);
	char output[OUTPUT_LENGTH];
	Init_block(output, OUTPUT_LENGTH);

	read_block(FILE_SIGN_AREA + index, temp);
	int file_length = temp[1];     //文件长度 
	int file_block = temp[2];     //文件实际使用块 
	int file_area;
	//**********************拷贝文件内容至temp_output 
	for (i = 0; i < file_block - 1; i++)
	{
		read_block(FILE_SIGN_AREA + index, temp);
		read_block(temp[3 + i], temp);
		strncpy(temp_output + i*B, temp, B);
	}
	read_block(FILE_SIGN_AREA + index, temp);
	read_block(temp[3 + i], temp);
	strncpy(temp_output + i*B, temp, B);
	//******************当前文件读写坐标
	int x = open_list[list].pointer[0];
	int y = open_list[list].pointer[1];

	for (i = 0; i < file_block; i++)
	{
		read_block(FILE_SIGN_AREA + index, temp);
		if (temp[3 + i] == x)
		{
			break;
		}
	}
	file_area = i * B + y;       //转换为文件内相对位置
	for (i = 0; i < count; i++)
	{
		output[i + mem_area] = temp_output[i + file_area];
	}
	printf("%s\n", output + mem_area);
	return OK;
}
int write(int index, int mem_area, int count)
/*
把mem_area 指定的内存位置开始的count 个字节顺序写入指定文件。
写操作从文件的读写指针指示的位置开始
*/
{
	int i;
	int list = -1;
	int input_length;
	char temp[B];
	//*************根据index找到文件打开表 
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].filesignnum == index)
		{
			list = i;
			break;
		}
	}
	if (list == -1)     //没找到 
	{
		printf("没找到当前索引号文件，操作失败\n");
		return ERROR;
	}
	if (open_list[list].flag != BUSY)  //输o?入¨?的ì?index对?应?|文?件t没?被à?打?¨°开a 
	{
		printf("输入的索引号有误，操作失败\n");
		return ERROR;
	}
	char input[INPUT_LENGTH];
	Init_block(input, INPUT_LENGTH);
	i = 0;
	fflush(stdin);
	while (scanf("%c", &input[i]))
	{
		if (input[i] == '\n')           //回车时终止读入
		{
			input[i] == '\0';
			break;
		}
		i++;
	}
	input_length = i;
	//*******************考虑输入串的长度需写入的长度为input_length - mem_area 
	//缓冲区容量可写入输入内容不需要再分配
	if (count <= BUFFER_LENGTH)
	{
		strncat(open_list[list].buffer, input + mem_area, count);   //存入缓冲区
	}
	//大于缓冲区长度，分次写入，需要分块写入
	else
	{
		int rest;      //当前缓冲区空闲容量 
		for (i = 0; i < BUFFER_LENGTH; i++)
		{
			if (open_list[list].buffer[i] == FREE)
			{
				rest = BUFFER_LENGTH - i;
				break;
			}
		}
		//第一部分缓冲区有一定容量rest 将缓冲区写入文件清空缓冲区
		strncat(open_list[list].buffer + BUFFER_LENGTH - rest, input + mem_area, rest);
		write_buffer(index, list);
		Init_block(open_list[list].buffer, BUFFER_LENGTH);
		//第二部分循环(input_length - mem_area)/BUFFER_LENGTH 块缓冲区写入文件
		for (i = 0; i < (count / BUFFER_LENGTH) - 1; i++)
		{
			strncpy(open_list[list].buffer, (input + mem_area) + rest + i*BUFFER_LENGTH, BUFFER_LENGTH);
			write_buffer(index, list);
			Init_block(open_list[list].buffer, BUFFER_LENGTH);
		}
		//第三部分(count%BUFFER_LENGTH)剩余未满块的写入缓冲区 
		Init_block(open_list[list].buffer, BUFFER_LENGTH);
		strncpy(open_list[list].buffer, (input + mem_area) + rest + i*BUFFER_LENGTH, count%BUFFER_LENGTH);
		int buffer_start;
	}
	return OK;
}

void directory()
//列表显示所有文件及其长度
{
	int i;
	int filenum;
	int filelength;
	char filename[FILE_NAME_LENGTH];
	char temp[B];
	char tempd[B];
	char temps[B];
	read_block(FILE_SIGN_AREA, temp);
	filenum = temp[1];      //实际存在的文件个数
	printf("\n");
	if (filenum == 0)
	{
		printf("\t\t\t\t该目录下没有文件t\n");
	}
	for (i = 0; i < FILE_NUM; i++)
	{
		read_block(temp[3 + i], tempd);     //读取目录项
		if (tempd[0] != 0)
		{
			read_block(tempd[0] + FILE_SIGN_AREA, temps);  //读取文件描述符
			if (temps[0] == BUSY && tempd[0] != 0)
			{
				filelength = temps[1];
				strcpy(filename, tempd + 1);
				printf("%-10s\t\t%-2d字节\n", filename, filelength);
			}
		}
	}

	if (filenum != 0)
	{
		printf("\t\t\t\t共2%d个文件\n", filenum);
	}
}

int show_openlist()
{

	int i, j;
	int openfile = 0;
	char temp[B];
	int index;
	printf("\n索引号大小\t\t文件名\n");
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].flag == BUSY)
		{
			index = open_list[i].filesignnum;
			printf("  %-2d", index);
			openfile++;
			read_block(FILE_SIGN_AREA + index, temp);
			printf("\t\t %-2d", temp[1]);  //根据index(文件描述符序号)找到其目录项 
			for (j = K; j < K + FILE_NUM; j++)
			{
				read_block(j, temp);
				if (temp[0] == index)
				{
					printf("\t\t%-10s\n", temp + 1);
				}
			}
		}
	}
	return openfile;
}
void show_help()
{
	
	printf(" dir                  显示目录内容                                     \n");
	//printf("                                                                       \n");
	printf(" create+filename      新建以filename为文件名的文件                     \n");
//	printf("                                                                       \n");
	printf(" delete+filename      删除以filename为文件名的文件                     \n");
	//printf("                                                                       \n");
	printf(" open+filename        打开以filename为文件名的文件                     \n");
	//printf("                                                                       \n");
	printf(" close                关闭index为索引号的文件                          \n");
	//printf("                                                                       \n");
	printf(" read                 请根据提示接着要输入索引号以及读取长进行读取文件操作       \n");
	//printf("                                                             \n");
	//printf("                                                                       \n");
	printf(" write                请根据提示接着要求输入索引号以及写入长度进行写入操作       \n");
	//printf("                                                             \n");
	//printf("                                                                       \n");
	printf(" lseek                请根据提示接着要求输入一个不大于文件长度的数字用于定位读写指针   \n");
	//printf("                                                       \n");
	//printf("                                                                       \n");
	printf(" help                 帮助                                             \n");
	//printf("                                                                       \n");
	printf(" exit                 退出文件系统                                     \n");
}

void show_ldisk()
{
	int a, b;
	for (a = 0; a < K + 30; a++)
	{
		printf("%-3d :", a);
		for (b = 0; b< B; b++)
		{
			printf("%-3d ", ldisk[a][b]);
		}
		printf("\n");
	}
}
