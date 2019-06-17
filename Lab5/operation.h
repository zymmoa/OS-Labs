#pragma once
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 


//��������
int show_openlist();    //��ʾ���ļ������ش��ļ�����
void directory();     //��ʾĿ¼�ļ���ϸ��Ϣ
void show_help();     //���ļ�ϵͳ�İ���
void show_ldisk();     //��ʾ�������ݸ�����
					   //���ĺ���
void read_block(int, char *);  //�ļ�ϵͳ��IO�豸�Ľӿں�����ȡ�� 
void write_block(int, char *);  //�ļ�ϵͳ��IO�豸�Ľӿں���д���
void Init();      //��ʼ���ļ�ϵͳ
int create(char *);     //�����ļ� 
int destroy(char *);    //ɾ���ļ� 
int open(char *);     //���ļ�
int close(int);      //�ر��ļ�
int read(int, int, int);    //���ļ�
int write(int, int, int);    //д�ļ� 
int write_buffer(int, int);   //�ѻ���������д���ļ� 
int lseek(int, int);     //��λ�ļ�ָ�� 
void Init_block(char, int);   //��ʼ���ַ�����鸨��

void read_block(int i, char *p) //�����̿�ú������߼�������ݶ��뵽ָ��pָ����ڴ�λ�ÿ������ַ�����Ϊ�洢��ĳ��� 
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

void write_block(int i, char *p) //д���̿�ú�����ָ��ָ�������д���߼��鿽�����ַ�����Ϊ�洢��ĳ���
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
			buffer_length = i;     //��������Ч���� 
			break;
		}
	}

	int x = open_list[list].pointer[0];
	int y = open_list[list].pointer[1];
	int z = B - y;         //��ǰ��������� 
						   // printf("W:buffer_length: %d  x:%d y:%d z:%d\n",buffer_length,x,y,z); 

	if (buffer_length < z)     //��������д�뻺��������Ҫ�ٷ��� 
	{
		read_block(x, temp);
		strncat(temp + y, open_list[list].buffer, buffer_length);   //����������
																	//printf("temp[0]: %c\n",temp[0]); 
		write_block(x, temp);

		read_block(index + FILE_SIGN_AREA, temp); //����1��?��?��t��?���ȡ��� 
												  //printf("temp[1] = %d\n",temp[1]); 
		temp[1] += buffer_length;
		//printf("temp[1] = %d\n",temp[1]); 
		write_block(index + FILE_SIGN_AREA, temp);

		open_list[list].pointer[0] = x;
		open_list[list].pointer[1] = y + buffer_length;     //�����ļ���дָ�� 
	}
	else         //������Ҫ�����¿� 
	{
		read_block(index + FILE_SIGN_AREA, temp);
		if (temp[2] + (buffer_length - z) / B + 1 > FILE_BLOCK_LENGTH)
		{
			printf("�ļ��������鲻������?\n");
			return ERROR;
		}

		//�ֱ������� 
		//��һ��������
		read_block(x, temp);
		strncat(temp + y, open_list[list].buffer, z);   //����������ĳ���������ǰ�� 
		write_block(x, temp);
		//�ڶ����ֻ���Ҫ����(buffer_length - z)/B+1��

		//Ѱ���ļ���Ŀ¼��֮��Ŀ��п�����¿�
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
				printf("������������ʧ��\n");
				return ERROR;
			}

			Init_block(temp, B);
			strncpy(temp, (open_list[list].buffer + z + (i*B)), B);
			write_block(freed, temp);    //д������ 

			read_block((freed - K) / B, temp);   //����λͼ״̬ 
			temp[(freed - K) % B] = BUSY;
			write_block((freed - K) / B, temp);   //д�����λͼ(�ļ���������Ӧ��) 
			read_block(index + FILE_SIGN_AREA, temp);
			temp[2] ++;        //�����ļ��������鳤�� 
			temp[2 + temp[2]] = freed;
			write_block(index + FILE_SIGN_AREA, temp);
		}
		//(buffer_length - z)%B ��������β���Ĳ���
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
			printf("������������ʧ��\n");
			return ERROR;
		}
		Init_block(temp, B);
		// printf("(buffer_length - z)%B = %d\n",(buffer_length - z)%B); 
		strncpy(temp, (open_list[list].buffer + z + (i*B)), (buffer_length - z) % B);
		write_block(freed, temp);    //д������ 
		read_block((freed - K) / B, temp);   //����λͼ״̬
		temp[(freed - K) % B] = BUSY;
		write_block((freed - K) / B, temp);   //д�����λͼ(�ļ���������Ӧ��) 
		read_block(index + FILE_SIGN_AREA, temp);
		temp[2] ++;        //�����ļ��������鳤�� 
		temp[2 + temp[2]] = freed;
		write_block(index + FILE_SIGN_AREA, temp);
		read_block(index + FILE_SIGN_AREA, temp);
		temp[1] += buffer_length;    //�����ļ����� 
		write_block(index + FILE_SIGN_AREA, temp);

		open_list[list].pointer[0] = freed;
		open_list[list].pointer[1] = (buffer_length - z) % B;
	}
	// printf("X = %d, Y = %d\n",open_list[list].pointer[0],open_list[list].pointer[1]); 
}
int lseek(int index, int pos)
/*
���ļ��Ķ�дָ���ƶ���posָ����λ�á�
pos��һ��������ʾ���ļ���ʼλ�õ�ƫ������
�ļ���ʱ��дָ���Զ�Ϊ0��
ÿ�ζ�д����֮����ָ����󱻷��ʵ��ֽڵ���һ��λ�á�
lseek �ܹ��ڲ����ж�д����������¸ı��дָ��λ�á�
*/
{
	int i;
	int list = -1;
	char temp[B];
	int pos_i = pos / B;    //���ļ����п����еĵ�X��
	int pos_j = pos % B;    //�ڵ�X����D��X��λ�� 
							//����Yindex�ұ�Ŀ
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].filesignnum == index)
		{
			list = i;
			break;
		}
	}

	if (list == -1)     //û�ҵ�
	{
		printf("û�ҵ���ǰ�������ļ�,����ʧ��\n");
		return ERROR;
	}
	if (open_list[list].flag != BUSY)  //�����index��Ӧ�ļ�û����
	{
		printf("���������������,����ʧ��\n");
		return ERROR;
	}
	//����Y��Ŀ�е�index���ļ����������ҵ���������
	read_block(open_list[list].filesignnum + FILE_SIGN_AREA, temp);
	if (pos_i > temp[2] - 1)           //�����ļ�ʵ�ʵĿ���ʱ
	{
		printf("�쳣Խ�磬��λʧ��\n");
		return ERROR;
	}

	//��λָ��
	open_list[list].pointer[0] = temp[3 + pos_i];    //���ļ��ĵ�X���ʵ�ʴ��̵�ַ 
	open_list[list].pointer[1] = pos_j;

	return OK;

}


void Init()
/*��ʼ�����̽�����ȫ���ÿ�Ȼ�󴴽�0���ļ�������Ϊ��Ŀ¼���ļ���������ʼ��λͼ��*/
{
	int i;
	char temp[B];


	for (i = 0; i < L; i++)
	{
		Init_block(temp, B);
		write_block(i, temp);   //��ʼ������
	}

	for (i = K; i < L; i++)      //��ʼ���������е�λͼ
	{
		read_block((i - K) / B, temp);
		temp[(i - K) % B] = FREE;
		write_block((i - K) % B, temp);
	}
	//******��ʼ��Ŀ¼���ļ���ʶ��
	filesign temp_cnt_sign;
	temp_cnt_sign.filesign_flag = 1;
	temp_cnt_sign.file_length = 0;     //�ļ����ȳ�ʼΪ0   
	temp_cnt_sign.file_block = FILE_BLOCK_LENGTH; //����FILE_BLOCK_LENGTH*B�Ŀռ���������ǰ�Ϊ����ڴ���Ŀ¼��

	Init_block(temp, B);
	temp[0] = temp_cnt_sign.filesign_flag;
	temp[1] = temp_cnt_sign.file_length;
	temp[2] = temp_cnt_sign.file_block;

	for (i = 0; i < FILE_BLOCK_LENGTH; i++)
	{
		temp[i + 3] = K + i;         //Ĭ����������ǰFILE_BLOCK_LENGTH����Ŀ¼ռ��
	}
	write_block(FILE_SIGN_AREA, temp);
	//**************����λͼ��״̬
	read_block(0, temp);
	for (i = 0; i < FILE_NUM; i++)
	{
		temp[i] = FREE;      //��ʱδ�����ļ�
	}
	write_block(0, temp);
}


int create(char filename[])
/**************************�����ļ��������ļ�
* ��һ�����ļ�������
* ���ļ�Ŀ¼��Ϊ�´������ļ�����һ��Ŀ¼�������ҪΪĿ¼�ļ������µĴ��̿�
* �ڷ��䵽��Ŀ¼�����¼�ļ����Լ����������
* ����״̬��Ϣ
***************************/
{
	int i;
	int frees;       //���е��ļ�������λ��  
	int freed;       //����������λ��Ŀ¼�� 
	int freed2;
	char temps[B];
	char tempc[B];
	char temp[B];
	//***************�鿴�ļ����Ƿ����
	for (i = K; i < K + FILE_NUM; i++)
	{
		read_block((i - K) / B, temp);
		if (temp[(i - K) % B] == BUSY)
		{
			read_block(i, temp);
			if (strncmp(temp + 1, filename, FILE_NAME_LENGTH) == 0)
			{
				printf("��Ŀ¼�Ѿ������ļ���Ϊ%s���ļ�\n", filename);
				return ERROR;
			}
		}
	}
	//***************Ѱ�ұ������п��е��ļ�������
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
		printf("û�п��е��ļ�������\n");
		return ERROR;
	}
	//****************Ѱ��������Ŀ¼��������ָ�����п��еĴ洢��
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
		printf("�ļ������Ѵ�����\n");
		return ERROR;
	}
	//******************Ѱ���ļ���Ŀ¼��֮��Ŀ��п������´������ļ� 
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
		printf("������������ʧ��\n");
		return ERROR;
	}
	//*****************�������ʼ���� 
	filesign temp_filesign;      //������ʱ�ļ������� 
	contents temp_contents;      //������ʱĿ¼��

								 //**************�����ļ�������
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
	write_block(frees, temps);    //д������ļ���������
								  //*****************����Ŀ¼�����Ŀ¼�ļ���������ָ���������
	temp_contents.filesignnum = frees - FILE_SIGN_AREA;
	strncpy(temp_contents.filename, filename, FILE_NAME_LENGTH);

	Init_block(tempc, B);
	tempc[0] = temp_contents.filesignnum;
	tempc[1] = '\0';
	strcat(tempc, temp_contents.filename);
	write_block(freed, tempc);    //д�����������
								  //*****************����λͼ״̬
	read_block((freed - K) / B, temp);   //����λͼ״̬(Ŀ¼������Ӧ��) 
	temp[(freed - K) % B] = BUSY;
	write_block((freed - K) / B, temp);   //д�����λͼ 

	read_block((freed2 - K) / B, temp);   //����λͼ״̬
	temp[(freed2 - K) % B] = BUSY;
	write_block((freed2 - K) / B, temp);   //д�����λͼ(�ļ���������Ӧ��) 

										   //****************����Ŀ¼�ļ��������еĳ����� 
	read_block(FILE_SIGN_AREA, temp);
	temp[1]++;
	write_block(FILE_SIGN_AREA, temp);

	return OK;

}

int destroy(char * filename)
/**************************ɾ��ָ���ļ�
* ��Ŀ¼���������ļ������������
* ɾ�����ļ���Ӧ��Ŀ¼�����λͼ
* �ͷ��ļ�������
* ����״̬��Ϣ
***************************/
{
	int i;
	int dtys;          //��Ҫɾ��y���ļ���Ŀ¼����ļ�������λ��
	int dtyd;          //��Ҫɾ��y���ļ���Ŀ¼��λ��
	int use_block;         //���ļ�ʵ��ʹ�õĿ���
	int index;
	char temp[B];
	char tempd[B];
	//***************�����ļ���Ѱ���ļ���Ŀ¼����ļ�������
	for (i = K; i < K + FILE_NUM; i++)
	{
		read_block((i - K) / B, temp);
		if (temp[(i - K) % B] == BUSY)
		{
			read_block(i, temp);
			if (strncmp(temp + 1, filename, FILE_NAME_LENGTH) == 0)
			{
				dtyd = i;        //�ҵ��ļ�Ŀ¼��λ��
				dtys = temp[0] + FILE_SIGN_AREA;  //�ҵ��ļ�Ŀ¼���Ӧ���ļ�������λ��(���Ϊatemp[0]) 
				index = temp[0];
				break;
			}
		}
	}
	if (i == K + FILE_NUM)
	{
		printf("û���ҵ����ļ�t\n");
		return ERROR;
	}
	//************�����ļ���������index�鿴���ļ��Ƿ��
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
		printf("���ļ��Ѿ�����,��Ҫ�رղ���ɾ��\n");
		return ERROR;
	}
	//****************�ҵ�λ�ú�ʼ���� 
	//****************���θ����ļ���������ָ���Ŀ�λͼ�� 
	read_block(dtys, temp);
	use_block = temp[2];
	for (i = 0; i < use_block; i++)
	{
		read_block((temp[i + 3] - K) / B, tempd);
		tempd[(temp[i + 3] - K) % B] = FREE;
		write_block((temp[i + 3] - K) / B, tempd);
	}
	//***************ɾ����Ŀ¼��
	Init_block(temp, B);
	write_block(dtys, temp);
	//***************ɾ���ļ�������
	Init_block(temp, B);
	write_block(dtyd, temp);

	//****************����λͼ�� 
	read_block((dtyd - K) / B, temp);
	temp[(dtyd - K) % B] = FREE;
	write_block((dtyd - K) / B, temp);
	//**************Ŀ¼�ļ��������еĳ��ȼ�1 
	read_block(FILE_SIGN_AREA, temp);
	temp[1]--;
	write_block(FILE_SIGN_AREA, temp);
	return OK;
}

int open(char * filename)
/***************************���ļ�
* �ú������ص������ſ����ں�����read, write, lseek, ��close������
* ����Ŀ¼�ҵ��ļ���Ӧ�����������
* �ڴ��ļ����з���һ����Ŀ
* �ڷ��䵽�ı�Ŀ�аѶ�дָ����Ϊ0����¼���������
* �����ļ��ĵ�һ�鵽��д��������
* ���ط��䵽�ı�Ŀ�ڴ��ļ����е�������
***************************/
{
	int i;
	int opd;
	int ops;
	int list;
	char temp[B];
	int index;
	//***************�����ļ���Ѱ���ļ���Ŀ¼����ļ�������
	for (i = K; i < K + FILE_NUM; i++)
	{
		read_block((i - K) / B, temp);
		if (temp[(i - K) % B] == BUSY)
		{
			read_block(i, temp);
			if (strncmp(temp + 1, filename, FILE_NAME_LENGTH) == 0)
			{
				opd = i;        //�ҵ��ļ�Ŀ¼��λ�� 
				ops = temp[0];       //�ҵ��ļ�Ŀ¼���Ӧ���ļ����������
									 // printf("opd: %d,ops: %d\n",opd,ops); 
				break;
			}
		}
	}
	if (i == K + FILE_NUM)
	{
		printf("û���ҵ����ļ�\n");
		return ERROR;
	}

	//*************�鿴���ļ��Ƿ񱻴� 
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].filesignnum == ops && open_list[i].flag == BUSY)
		{
			printf("���ļ��Ѿ�����\n");
			return ERROR;
		}
	}

	//**************��һ��ûʹ�õı�Ŀ�Ա����
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].flag != BUSY)
		{
			list = i;
			break;
		}
	}

	//******************�Ա�Ŀ���в���

	open_list[list].filesignnum = ops;        //д���ļ���������� 

	open_list[list].flag = BUSY;         //�ñ�־λΪռ��

	index = open_list[list].filesignnum;       //����������(�൱�ڸ��ļ�Ŀ¼����ļ����������)   
	lseek(index, 0);             //ָ��ָ���ļ��ײ�(��ָ��ָΪ0) 

	Init_block(open_list[list].buffer, BUFFER_LENGTH);    //��ʼ��������
	read_block(open_list[list].pointer[0], temp);     //���ļ����׿�
	strncpy(open_list[list].buffer, temp, BUFFER_LENGTH);    //���ļ��ĵ�һ��д�뻺���� 
	return OK;
}

int close(int index)
/***************************�ر��ļ�
* �ѻ�����������д�����
* �ͷŸ��ļ��ٴ��ļ����ж�Ӧ�ı�Ŀ
* ����״̬��Ϣ
* ***************************/
{
	int i;
	int list = -1;
	char temp[B];
	//***************����Yindex�ұ�Ŀ 
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].filesignnum == index)
		{
			list = i;
			break;
		}
	}
	if (list == -1)     //û�ҵ� 
	{
		printf("û�ҵ���ǰ�������ļ�������ʧ��\n");
		return ERROR;
	}
	if (open_list[list].flag != BUSY)  //�����index��Ӧ�ļ�û����
	{
		printf("��������������󣬲���ʧ��\n");
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
��ָ���ļ�˳�����count���ֽ�mem_areaָ�����ڴ�λ�á�
���������ļ��Ķ�дָ��ָʾ��λ�ÿ�ʼ
*/
{
	int i;
	int list = -1;
	char temp[B];
	//***************����index�Ҵ��ļ����Ŀ
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].filesignnum == index)
		{
			list = i;
			break;
		}
	}
	if (list == -1)     //û�ҵ�
	{
		printf("û�ҵ���ǰ�������ļ�������ʧ��\n");
		return ERROR;
	}
	if (open_list[list].flag != BUSY)  //�����index��Ӧ�ļ�û����
	{
		printf("��������������󣬲���ʧ��\n");
		return ERROR;
	}
	//***************����index���ļ������� 
	char temp_output[OUTPUT_LENGTH];
	Init_block(temp_output, OUTPUT_LENGTH);
	char output[OUTPUT_LENGTH];
	Init_block(output, OUTPUT_LENGTH);

	read_block(FILE_SIGN_AREA + index, temp);
	int file_length = temp[1];     //�ļ����� 
	int file_block = temp[2];     //�ļ�ʵ��ʹ�ÿ� 
	int file_area;
	//**********************�����ļ�������temp_output 
	for (i = 0; i < file_block - 1; i++)
	{
		read_block(FILE_SIGN_AREA + index, temp);
		read_block(temp[3 + i], temp);
		strncpy(temp_output + i*B, temp, B);
	}
	read_block(FILE_SIGN_AREA + index, temp);
	read_block(temp[3 + i], temp);
	strncpy(temp_output + i*B, temp, B);
	//******************��ǰ�ļ���д����
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
	file_area = i * B + y;       //ת��Ϊ�ļ������λ��
	for (i = 0; i < count; i++)
	{
		output[i + mem_area] = temp_output[i + file_area];
	}
	printf("%s\n", output + mem_area);
	return OK;
}
int write(int index, int mem_area, int count)
/*
��mem_area ָ�����ڴ�λ�ÿ�ʼ��count ���ֽ�˳��д��ָ���ļ���
д�������ļ��Ķ�дָ��ָʾ��λ�ÿ�ʼ
*/
{
	int i;
	int list = -1;
	int input_length;
	char temp[B];
	//*************����index�ҵ��ļ��򿪱� 
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].filesignnum == index)
		{
			list = i;
			break;
		}
	}
	if (list == -1)     //û�ҵ� 
	{
		printf("û�ҵ���ǰ�������ļ�������ʧ��\n");
		return ERROR;
	}
	if (open_list[list].flag != BUSY)  //��o?�롧?�Ĩ�?index��?Ӧ?|��?��tû?����?��?���㿪a 
	{
		printf("��������������󣬲���ʧ��\n");
		return ERROR;
	}
	char input[INPUT_LENGTH];
	Init_block(input, INPUT_LENGTH);
	i = 0;
	fflush(stdin);
	while (scanf("%c", &input[i]))
	{
		if (input[i] == '\n')           //�س�ʱ��ֹ����
		{
			input[i] == '\0';
			break;
		}
		i++;
	}
	input_length = i;
	//*******************�������봮�ĳ�����д��ĳ���Ϊinput_length - mem_area 
	//������������д���������ݲ���Ҫ�ٷ���
	if (count <= BUFFER_LENGTH)
	{
		strncat(open_list[list].buffer, input + mem_area, count);   //���뻺����
	}
	//���ڻ��������ȣ��ִ�д�룬��Ҫ�ֿ�д��
	else
	{
		int rest;      //��ǰ�������������� 
		for (i = 0; i < BUFFER_LENGTH; i++)
		{
			if (open_list[list].buffer[i] == FREE)
			{
				rest = BUFFER_LENGTH - i;
				break;
			}
		}
		//��һ���ֻ�������һ������rest ��������д���ļ���ջ�����
		strncat(open_list[list].buffer + BUFFER_LENGTH - rest, input + mem_area, rest);
		write_buffer(index, list);
		Init_block(open_list[list].buffer, BUFFER_LENGTH);
		//�ڶ�����ѭ��(input_length - mem_area)/BUFFER_LENGTH �黺����д���ļ�
		for (i = 0; i < (count / BUFFER_LENGTH) - 1; i++)
		{
			strncpy(open_list[list].buffer, (input + mem_area) + rest + i*BUFFER_LENGTH, BUFFER_LENGTH);
			write_buffer(index, list);
			Init_block(open_list[list].buffer, BUFFER_LENGTH);
		}
		//��������(count%BUFFER_LENGTH)ʣ��δ�����д�뻺���� 
		Init_block(open_list[list].buffer, BUFFER_LENGTH);
		strncpy(open_list[list].buffer, (input + mem_area) + rest + i*BUFFER_LENGTH, count%BUFFER_LENGTH);
		int buffer_start;
	}
	return OK;
}

void directory()
//�б���ʾ�����ļ����䳤��
{
	int i;
	int filenum;
	int filelength;
	char filename[FILE_NAME_LENGTH];
	char temp[B];
	char tempd[B];
	char temps[B];
	read_block(FILE_SIGN_AREA, temp);
	filenum = temp[1];      //ʵ�ʴ��ڵ��ļ�����
	printf("\n");
	if (filenum == 0)
	{
		printf("\t\t\t\t��Ŀ¼��û���ļ�t\n");
	}
	for (i = 0; i < FILE_NUM; i++)
	{
		read_block(temp[3 + i], tempd);     //��ȡĿ¼��
		if (tempd[0] != 0)
		{
			read_block(tempd[0] + FILE_SIGN_AREA, temps);  //��ȡ�ļ�������
			if (temps[0] == BUSY && tempd[0] != 0)
			{
				filelength = temps[1];
				strcpy(filename, tempd + 1);
				printf("%-10s\t\t%-2d�ֽ�\n", filename, filelength);
			}
		}
	}

	if (filenum != 0)
	{
		printf("\t\t\t\t��2%d���ļ�\n", filenum);
	}
}

int show_openlist()
{

	int i, j;
	int openfile = 0;
	char temp[B];
	int index;
	printf("\n�����Ŵ�С\t\t�ļ���\n");
	for (i = 0; i < FILE_NUM; i++)
	{
		if (open_list[i].flag == BUSY)
		{
			index = open_list[i].filesignnum;
			printf("  %-2d", index);
			openfile++;
			read_block(FILE_SIGN_AREA + index, temp);
			printf("\t\t %-2d", temp[1]);  //����index(�ļ����������)�ҵ���Ŀ¼�� 
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
	
	printf(" dir                  ��ʾĿ¼����                                     \n");
	//printf("                                                                       \n");
	printf(" create+filename      �½���filenameΪ�ļ������ļ�                     \n");
//	printf("                                                                       \n");
	printf(" delete+filename      ɾ����filenameΪ�ļ������ļ�                     \n");
	//printf("                                                                       \n");
	printf(" open+filename        ����filenameΪ�ļ������ļ�                     \n");
	//printf("                                                                       \n");
	printf(" close                �ر�indexΪ�����ŵ��ļ�                          \n");
	//printf("                                                                       \n");
	printf(" read                 �������ʾ����Ҫ�����������Լ���ȡ�����ж�ȡ�ļ�����       \n");
	//printf("                                                             \n");
	//printf("                                                                       \n");
	printf(" write                �������ʾ����Ҫ�������������Լ�д�볤�Ƚ���д�����       \n");
	//printf("                                                             \n");
	//printf("                                                                       \n");
	printf(" lseek                �������ʾ����Ҫ������һ���������ļ����ȵ��������ڶ�λ��дָ��   \n");
	//printf("                                                       \n");
	//printf("                                                                       \n");
	printf(" help                 ����                                             \n");
	//printf("                                                                       \n");
	printf(" exit                 �˳��ļ�ϵͳ                                     \n");
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
