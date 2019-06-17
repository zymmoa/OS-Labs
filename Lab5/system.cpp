#include <stdio.h> 
#include <stdlib.h> 
#include <string>
#include<cstdio>
//#include <iostream.h> 
using namespace std;

#include "Define.h" 
#include "Operation.h" 

#define DIR  0 
#define CREATE 1 
#define DELETE 2 
#define OPEN 3 
#define CLOSE 4 
#define READ 5  
#define WRITE 6 
#define HELP 7 
#define LSEEK 8 
#define EXIT 9 
#define OPLIST 10 
#define LDISK 11 
#define CH_LENGTH 20 


int main()
{
	printf("\t\tzym�ļ�ϵͳ ��������\n");
	show_help();
	Init();
	create("zym1");
	create("zym2");
	create("zym3");
	open("zym1");
	open("zym3");
	char ch[CH_LENGTH];
	Init_block(ch, CH_LENGTH);
	while (gets(ch))
	{
		int cmd;
		char filename[FILE_NAME_LENGTH];
		//��ʼ�� 
		cmd = -1;
		Init_block(filename, FILE_NAME_LENGTH);
		if (strncmp("dir", ch, 3) == 0)   //���Ŀ¼dir(non) 
		{
			cmd = DIR;
		}
		if (strncmp("create", ch, 6) == 0)   //�����ļ�����create(filename) 
		{
			cmd = CREATE;
			strcat(filename, ch + 7);
		}
		if (strncmp("delete", ch, 6) == 0)   //ɾ���ļ�����delete(filename) 
		{
			cmd = DELETE;
			strcat(filename, ch + 7);
		}
		if (strncmp("open", ch, 4) == 0)   //���ļ�����open(filename) 
		{
			cmd = OPEN;
			strcat(filename, ch + 5);
		}
		if (strncmp("close", ch, 5) == 0)   //�ر��ļ�����close(index) 
		{
			cmd = CLOSE;
		}
		if (strncmp("read", ch, 4) == 0)   //���ļ�����read(index) 
		{
			cmd = READ;
		}
		if (strncmp("write", ch, 5) == 0)   //д�ļ�����write(index) 
		{
			cmd = WRITE;
		}
		if (strncmp("lseek", ch, 5) == 0)   //ָ������lseek(index,pos) 
		{
			cmd = LSEEK;
		}
		if (strncmp("oplist", ch, 6) == 0)   //�鿴�ļ���
		{
			cmd = OPLIST;
		}
		if (strncmp("exit", ch, 4) == 0)   //�˳�����exit 
		{
			cmd = EXIT;
			break;
		}
		if (strncmp("ldisk", ch, 5) == 0)   //�鿴Ӳ������
		{
			cmd = LDISK;
		}
		if (strncmp("help", ch, 4) == 0)   //��������help(non) 
		{
			cmd = HELP;
		}
		int index, count, pos;
		switch (cmd)
		{
		case DIR:
			directory();
			printf("----------------------------------------------\n");
			break;
		case CREATE:
			if (create(filename) == OK)
				printf("�����ļ��ɹ�\n");
			printf("----------------------------------------------\n");
			break;
		case DELETE:
			if (destroy(filename) == OK)
				printf("ɾ���ļ��ɹ�\n");
			printf("----------------------------------------------\n");
			break;
		case OPEN:
			if (open(filename) == OK)
				printf("���ļ��ɹ�\n");
			printf("----------------------------------------------\n");
			break;
		case CLOSE:
			if (show_openlist() == 0)
			{
				printf("��ǰû���ļ�����\n");
				("----------------------------------------------\n");
				break;
			}
			printf("������Ҫ�ر��ļ���������\n");
			scanf("%d", &index);
			if (close(index) == OK)
				printf("�رղ����ɹ�\n");
			printf("----------------------------------------------\n");
			break;
		case READ:
			if (show_openlist() == 0)
			{
				printf("��ǰû���ļ�����\n");
				printf("----------------------------------------------\n");
				break;
			}
			printf("������Ҫ��ȡ�ļ���������\n");
			scanf("%d", &index);
			printf("��������ҪҪ��ȡ�ļ�����\n");
			scanf("%d", &count);
			if (read(index, 0, count) == OK)
				printf("���ļ������ɹ�\n");
			printf("----------------------------------------------\n");
			break;
		case WRITE:
			if (show_openlist() == 0)
			{
				printf("��ǰû���ļ�����\n");
				printf("----------------------------------------------\n");
				break;
			}
			printf("������Ҫд���ļ���������\n");
			scanf("%d", &index);
			printf("��������Ҫд���ļ��ĳ���\n");
			scanf("%d", &count);
			if (write(index, 0, count) == OK)
				printf("д������ɹ�\n");
			printf("----------------------------------------------\n");
			break;
		case LSEEK:
			if (show_openlist() == 0)
			{
				printf("��ǰû���ļ�����\n");
				printf("----------------------------------------------\n");
				break;
			}
			printf("������Ҫд���ļ���������\n");
			scanf("%d", &index);
			printf("��������Ҫ���õ��ļ����λ��\n");
			scanf("%d", &pos);
			lseek(index, pos);
			printf("----------------------------------------------\n");
			break;
		case OPLIST:
			if (show_openlist() == 0)
			{
				printf("\t\t\n��ǰû���ļ�����\n");
				printf("----------------------------------------------\n");
				break;
			}
			printf("----------------------------------------------\n");
			break;
		case HELP:
			show_help();
			break;
		case LDISK:
			show_ldisk();
			break;
		default:
			printf("ָ�����\n");
			printf("----------------------------------------------\n");
			break;
		}
		fflush(stdin);
		Init_block(ch, CH_LENGTH);
	}
	//return 0�� 
}
