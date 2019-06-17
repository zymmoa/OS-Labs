#pragma once
#define B  10   //存储块长度 
#define L  500   //存储块总数
#define K  100   //保留区大小
#define BUSY 1
#define FREE 0 
#define OK  1 
#define ERROR 0 
#define FILE_BLOCK_LENGTH  (B-3)    //文件分配磁盘块号数组长度 
#define FILE_NAME_LENGTH  (B-1)    //最长文件名长度
#define FILE_SIGN_AREA   ((L-1-K)/B+1)  //保留区中文件标识符起始块号(位图之后) 
#define FILE_NUM    FILE_BLOCK_LENGTH //目录内最多文件数目 
#define BUFFER_LENGTH   25     //打开文件表目中的缓冲区长度 
#define INPUT_LENGTH   100     //写文件时最大输入长度 
#define OUTPUT_LENGTH   100     //读文件时最大读出长度 



struct filesign {       //文件描述符
	int file_length;      //文件长度
	int filesign_flag;      //占用标识位
	int file_block;       //文件分配磁盘号 数组实际长度 
	int file_block_ary[FILE_BLOCK_LENGTH]; //文件分配磁盘块
};

struct contents {       //目目录项
	char filename[FILE_NAME_LENGTH];  //文件名
	int  filesignnum;      //文件描述符序号
};

struct openfilelist {      //打开文件表表目
	char buffer[BUFFER_LENGTH];    //读写缓冲区 
	int pointer[2];       //读写指针（文件内容的位置）
	int filesignnum;      //文件描述符
	int flag;        //占用符
};

char ldisk[L][B];      //用字符数组模拟磁盘

openfilelist open_list[FILE_NUM];  //打开文件表
