#pragma once
#define B  10   //�洢�鳤�� 
#define L  500   //�洢������
#define K  100   //��������С
#define BUSY 1
#define FREE 0 
#define OK  1 
#define ERROR 0 
#define FILE_BLOCK_LENGTH  (B-3)    //�ļ�������̿�����鳤�� 
#define FILE_NAME_LENGTH  (B-1)    //��ļ�������
#define FILE_SIGN_AREA   ((L-1-K)/B+1)  //���������ļ���ʶ����ʼ���(λͼ֮��) 
#define FILE_NUM    FILE_BLOCK_LENGTH //Ŀ¼������ļ���Ŀ 
#define BUFFER_LENGTH   25     //���ļ���Ŀ�еĻ��������� 
#define INPUT_LENGTH   100     //д�ļ�ʱ������볤�� 
#define OUTPUT_LENGTH   100     //���ļ�ʱ���������� 



struct filesign {       //�ļ�������
	int file_length;      //�ļ�����
	int filesign_flag;      //ռ�ñ�ʶλ
	int file_block;       //�ļ�������̺� ����ʵ�ʳ��� 
	int file_block_ary[FILE_BLOCK_LENGTH]; //�ļ�������̿�
};

struct contents {       //ĿĿ¼��
	char filename[FILE_NAME_LENGTH];  //�ļ���
	int  filesignnum;      //�ļ����������
};

struct openfilelist {      //���ļ����Ŀ
	char buffer[BUFFER_LENGTH];    //��д������ 
	int pointer[2];       //��дָ�루�ļ����ݵ�λ�ã�
	int filesignnum;      //�ļ�������
	int flag;        //ռ�÷�
};

char ldisk[L][B];      //���ַ�����ģ�����

openfilelist open_list[FILE_NUM];  //���ļ���
