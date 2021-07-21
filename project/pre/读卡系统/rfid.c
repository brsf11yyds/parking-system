#include "rfid.h"
unsigned char RBuf[128];
unsigned char KEYA_BUF[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
unsigned char KEYB_BUF[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

/* ���ô��ڲ���:9600���� */
void init_tty(int fd)
{    
	//�������ô��ڵĽṹ��
	struct termios termios_new;
	//����ոýṹ��
	bzero( &termios_new, sizeof(termios_new));
	//	cfmakeraw()�����ն����ԣ���������termios�ṹ�еĸ���������
	cfmakeraw(&termios_new);
	//���ò�����
	//termios_new.c_cflag=(B9600);
	cfsetispeed(&termios_new, B9600);
	cfsetospeed(&termios_new, B9600);
	//CLOCAL��CREAD�ֱ����ڱ������Ӻͽ���ʹ�ܣ���ˣ�����Ҫͨ��λ����ķ�ʽ����������ѡ�    
	termios_new.c_cflag |= CLOCAL | CREAD;
	//ͨ��������������λΪ8λ
	termios_new.c_cflag &= ~CSIZE;
	termios_new.c_cflag |= CS8; 
	//��������żУ��
	termios_new.c_cflag &= ~PARENB;
	//һλֹͣλ
	termios_new.c_cflag &= ~CSTOPB;
	tcflush(fd,TCIFLUSH);
	// �����ý����ַ��͵ȴ�ʱ�䣬������Ҫ����Խ�������Ϊ0
	termios_new.c_cc[VTIME] = 10;
	termios_new.c_cc[VMIN] = 1;
	// �����������/���������
	tcflush (fd, TCIFLUSH);
	//������ú󣬿���ʹ�����º������������
	if(tcsetattr(fd,TCSANOW,&termios_new) )
		printf("Setting the serial1 failed!\n");

}

/*����У���*/
unsigned char CalBCC(unsigned char *buf, int n)
{
	int i;
	unsigned char bcc=0;
	for(i = 0; i < n; i++)
	{
		bcc ^= *(buf+i);
	}
	return (~bcc);
}

/*����*/
int PiccRequest(int fd)
{
	unsigned char WBuf[128], RBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x07;	//֡��= 7 Byte
	WBuf[1] = 0x82;	//����= 0 , ��������= 0x02
	WBuf[2] = 'A';	//����= 'A'
	WBuf[3] = 0x01;	//��Ϣ����= 1
	WBuf[4] = 0x52;	//����ģʽ:  ALL=0x52
	WBuf[5] = CalBCC(WBuf, WBuf[0]-2);		//У���
	WBuf[6] = 0x03; 	//������־

        timeout.tv_sec = 300;
        timeout.tv_usec = 0;
		
	FD_ZERO(&rdfd);
	FD_SET(fd,&rdfd);

	write(fd, WBuf, 7);
	ret = select(fd + 1,&rdfd, NULL,NULL,&timeout);
 	printf("3 PiccRequest......................\n");
	switch(ret)
	{
		case -1:
			perror("PiccRequest select error\n");
			break;
		case  0:
			printf("PiccRequest timed out.\n");
			break;
		default:
			ret = read(fd, RBuf, 8);
			if (ret < 0)
			{
				printf("PiccRequest ret = %d, %d\n", ret, errno);
				break;
			}
			if (RBuf[2] == 0x00)	 	//Ӧ��֡״̬����Ϊ0 ������ɹ�  ���ؿ�������
			{
			        DataBfr[0] = RBuf[4];
			        DataBfr[1] = RBuf[5];
			        FD_CLR(fd,&rdfd);
			        printf("PiccRequest success\n");
				return 0;
			}
			break;
	}
	FD_CLR(fd,&rdfd);
	return 1;
}

/*����ײ����ȡ��Χ�����ID*/
int PiccAnticoll(int fd)
{
	unsigned char WBuf[128], RBuf[128];
	int ret, i;
	fd_set rdfd;
	struct timeval timeout;
	unsigned int ID;

#if 0
	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x0C;	//֡��= 8 Byte
	WBuf[1] = 0xC2;	//����= 0 , ��������= 0x02
	WBuf[2] = 'B';	//����= 'B'
	WBuf[3] = 0x06;	//��Ϣ����= 2
	WBuf[4] = 0x93;	//����ײ0x93 --һ������ײ
	WBuf[5] = 0x0A;	//λ����0
	WBuf[6] = 0xE2;
	WBuf[7] = 0xB5;
	WBuf[8] = 0x5D;
	WBuf[9] = 0xBB;
	WBuf[10] = CalBCC(WBuf, WBuf[0]-2);		//У���
	WBuf[11] = 0x03; 	//������־
#endif

#if 1
	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x08;	//֡��= 8 Byte
	WBuf[1] = 0x02;	//����= 0 , ��������= 0x02
	WBuf[2] = 'B';	//����= 'B'
	WBuf[3] = 0x02;	//��Ϣ����= 2
	WBuf[4] = 0x93;	//����ײ0x93 --һ������ײ
	WBuf[5] = 0x00;	//λ����0
	WBuf[6] = CalBCC(WBuf, WBuf[0]-2);		//У���
	WBuf[7] = 0x03; 	//������־
#endif 

	timeout.tv_sec = 300;
	timeout.tv_usec = 0;
	FD_ZERO(&rdfd);
	FD_SET(fd,&rdfd);
	write(fd, WBuf, 8);

	ret = select(fd + 1,&rdfd, NULL,NULL,&timeout);
	switch(ret)
	{
		case -1:
			perror("PiccAnticoll select error\n");
			break;
		case  0:
			perror("PiccAnticoll Timeout:");
			break;
		default:
			ret = read(fd, RBuf, 10);
			if (ret < 0)
			{
				printf("PiccAnticoll ret = %d, %d\n", ret, errno);
				break;
			}
			if (RBuf[2] == 0x00) //Ӧ��֡״̬����Ϊ0 ���ȡID �ɹ�
			{
				ID = (RBuf[7]<<24) | (RBuf[6]<<16) | (RBuf[5]<<8) | RBuf[4];
				
				memcpy(cardid,&RBuf[4],4);
				printf("PiccAnticoll The card ID is %x\n",ID);
				return 0;
			}
	}
	return -1;
}

/*ѡ��*/
int PiccSelect(int fd)
{
	unsigned char WBuf[128], RBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x0B;	//֡��= 7 Byte
	WBuf[1] = 0x02;	//����= 0 , ��������= 0x02
	WBuf[2] = 'C';	//����= 'C
	WBuf[3] = 0x05;	//��Ϣ����= 5
	WBuf[4] = 0x93;	//����ģʽ:  ALL=0x52
        memcpy(&WBuf[5],cardid,4);
	WBuf[9] = CalBCC(WBuf, WBuf[0]-2);		//У���
	WBuf[10] = 0x03; 	//������־

        timeout.tv_sec = 300;
        timeout.tv_usec = 0;
	FD_ZERO(&rdfd);
	FD_SET(fd,&rdfd);
	
	write(fd, WBuf, WBuf[0]);
	ret = select(fd + 1,&rdfd, NULL,NULL,&timeout);
	switch(ret)
	{
		case -1:
			perror("PiccSelect error\n");
			break;
		case  0:
			printf("PiccSelect timed out.\n");
			break;
		default:
			ret = read(fd, RBuf, 7);
			if (ret < 0)
			{
				printf("PiccSelect  ret = %d, %d\n", ret, errno);
				break;
			}
			if (RBuf[2] == 0x00)	 	//Ӧ��֡״̬����Ϊ0 ������ɹ�
			{
			        printf("PiccSelect success\n");
				return 0;
			}
			break;
	}
	return -1;
}

/*��ͣ*/
int PiccHalt(int fd)
{
	unsigned char WBuf[128], RBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x06;	//֡��= 6 Byte
	WBuf[1] = 0x02;	//����= 0 , ��������= 0x02
	WBuf[2] = 'D';	//����= 'D
	WBuf[3] = 0x00;	//��Ϣ����= 0
	WBuf[4] = CalBCC(WBuf, WBuf[0]-2);		//У���
	WBuf[5] = 0x03; 	//������־

        timeout.tv_sec = 300;
        timeout.tv_usec = 0;
	FD_ZERO(&rdfd);
	FD_SET(fd,&rdfd);
	
	write(fd, WBuf, WBuf[0]);
	ret = select(fd + 1,&rdfd, NULL,NULL,&timeout);
	switch(ret)
	{
		case -1:
			perror("PiccHalt error\n");
			break;
		case  0:
			printf("PiccHalt timed out.\n");
			break;
		default:
			ret = read(fd, RBuf, 7);
			if (ret < 0)
			{
				printf("PiccHalt  ret = %d, %d\n", ret, errno);
				break;
			}
			if (RBuf[2] == 0x00)	 	//Ӧ��֡״̬����Ϊ0 ������ɹ�
			{
			        printf("PiccHalt success\n");
				return 0;
			}
			break;
	}
	return -1;
}

/*������֤*/
int PiccAuthKey(int fd,int sector,unsigned char KeyAB)
{
	unsigned char WBuf[128], RBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x12;	//֡��= 17 Byte
	WBuf[1] = 0x02;	//����= 0 , ��������= 0x02
	WBuf[2] = 'F';	//����= 'F'
	WBuf[3] = 12;	//��Ϣ����= 0x0c
	WBuf[4] = KeyAB;
        memcpy(&WBuf[5],cardid,4);
        memcpy(&WBuf[9],KEYA_BUF,6);
        WBuf[15]=sector;   //��д�ĸ���
	WBuf[16] = CalBCC(WBuf, WBuf[0]-2);		//У���
	WBuf[17] = 0x03; 	//������־

        timeout.tv_sec = 300;
        timeout.tv_usec = 0;
	FD_ZERO(&rdfd);
	FD_SET(fd,&rdfd);

	write(fd, WBuf, WBuf[0]);
	ret = select(fd + 1,&rdfd, NULL,NULL,&timeout);
	switch(ret)
	{
		case -1:
			perror("PiccAuthKey error\n");
			break;
		case  0:
			printf("PiccAuthKey timed out.\n");
			break;
		default:
			ret = read(fd, RBuf, 7);
			if (ret < 0)
			{
				printf("PiccAuthKey  ret = %d, %d\n", ret, errno);
				break;
			}
			if (RBuf[2] == 0x00)	 	//Ӧ��֡״̬����Ϊ0 ������ɹ�
			{
			        printf("PiccAuthKey success\n");
				return 0;
			}
			break;
	}
	return -1;
}

/*����*/
int PiccRead(int fd,unsigned char sector)
{
	unsigned char WBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x07;	//֡��= 7 Byte
	WBuf[1] = 0x02;	//����= 0 , ��������= 0x02
	WBuf[2] = 'G';	//����= 'g
	WBuf[3] = 0x01;	//��Ϣ����= 1
	WBuf[4] = sector;	//���ĸ�����
	WBuf[5] = CalBCC(WBuf, WBuf[0]-2);		//У���
	WBuf[6] = 0x03; 	//������־

        timeout.tv_sec = 300;
        timeout.tv_usec = 0;
	FD_ZERO(&rdfd);
	FD_SET(fd,&rdfd);

	write(fd, WBuf, WBuf[0]);
	ret = select(fd + 1,&rdfd, NULL,NULL,&timeout);
	switch(ret)
	{
		case -1:
			perror("PiccRead error\n");
			break;
		case  0:
			printf("PiccRead timed out.\n");
			break;
		default:
			ret = read(fd, RBuf, 23);
			if (ret < 0)
			{
				printf("PiccRead  ret = %d, %d\n", ret, errno);
				break;
			}
			if (RBuf[2] == 0x00)	 	//Ӧ��֡״̬����Ϊ0 ������ɹ�
			{
			        usleep(10*1000);
			        read(fd,&RBuf[ret],6);
			        memcpy(DataReadBuf,&RBuf[4],16); //��ȡ��������������
			        for(i=0;i<23;i++)
			            printf("PiccRead RBuf[%d]=%x\n",i,RBuf[i]);
				return 0;
			}
			break;
	}
	return -1;
}

/*д��*/
int PiccWrite(int fd,unsigned char sector)
{
	unsigned char WBuf[128], RBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 23;	//֡��= 23 Byte
	WBuf[1] = 0x02;	//����= 0 , ��������= 0x02
	WBuf[2] = 'H';	//����= 'H
	WBuf[3] = 0X11;	//��Ϣ����= 17
	WBuf[4] = sector;   //д�ĸ�����
        memcpy(&WBuf[5],DataWriteBuf,16);
	WBuf[21] = CalBCC(WBuf, WBuf[0]-2);		//У���
	WBuf[22] = 0x03; 	//������־

        timeout.tv_sec = 300;
        timeout.tv_usec = 0;
	FD_ZERO(&rdfd);
	FD_SET(fd,&rdfd);
        
	write(fd, WBuf, 23);
	ret = select(fd + 1,&rdfd, NULL,NULL,&timeout);
	switch(ret)
	{
		case -1:
			perror("PiccWrite error\n");
			break;
		case  0:
			printf("PiccWrite timed out.\n");
			break;
		default:
			ret = read(fd, RBuf, 7);
			if (ret < 0)
			{
				printf("PiccWrite  ret = %d, %d\n", ret, errno);
				break;
			}
			if (RBuf[2] == 0x00)	 	//Ӧ��֡״̬����Ϊ0 ������ɹ�
			{
			        printf("PiccWrite card success\n");
				return 0;
			}
			break;
	}
	return -1;
}



/*RFID��ѯ��ֵ*/
int Picc_Menu(void)
{
	//1, ��ʼ�� --> �򿪴���, ��������
	int ret, i;
	int fd;
	int cmd_flag;
	int cont;
	int money;		//���
	unsigned char sector;

	fd = open(DEV_PATH, O_RDWR | O_NOCTTY | O_NONBLOCK);		//�򿪴���2  --> con3
	if (fd < 0)
	{
		fprintf(stderr, "Open Gec6818_ttySAC2 fail!\n");
		return -1;
	}
	/*��ʼ������*/
	init_tty(fd);

  	//ѡ������д���ĸ���
	sector = 1;	

	//2, RFID���Ĳ��� --> ���� -->����ײ -->ѡ�� -->��֤��Կ --> ѡ��"��ѯ","��ֵ"
	while(1)
	{
		/*�������߷�Χ�Ŀ�*/
		if ( PiccRequest(fd) )
		{
			sleep(1);
			printf("����ʧ��!\n");
			continue;
		}	
		printf("����ɹ�!\n");
		usleep(30*1000);	
		/*���з���ײ����ȡ���߷�Χ������ID*/
		if( PiccAnticoll(fd) )
		{
			printf("����ײʧ��!\n");
			continue;
		}		
		usleep(30*1000);
		/*����*/
		if( PiccSelect(fd) )
		{
			printf("����ʧ��!\n");
			continue;
		}		
		usleep(30*1000);
		/*������֤ : ��֤��Կ*/
		if( PiccAuthKey( fd,  sector, KEYA))
		{
			printf("4 PiccAuthKey KEYA failed now try KEYB\n");
				
			/*�������߷�Χ�Ŀ�*/
			if ( PiccRequest(fd) )
			{
				printf("5 The request failed!\n");
				continue;
			}
			/*���з���ײ����ȡ���߷�Χ������ID*/
			if( PiccAnticoll(fd) )
			{
				printf("6 RFID PiccAnticoll failed\n");
				continue;
			}
			/*����*/
			if( PiccSelect(fd) )
			{
					printf("7 PiccSelect failed\n");
					continue;
			}
			/*������֤*/
			if( PiccAuthKey( fd,  sector, KEYB) )
				printf("8PiccAuthKey failed\n");
				continue;
		}	
		
		/*��ȡRFID������1������*/
		if( PiccRead(fd , sector) )	
		{
			printf("==��ȡʧ��==\n");
			continue;
		}
		printf("\n*******ѡ����Ҫִ�еĲ���: 0 ��ѯ; 1 ��ֵ; 2 ����; 3 �˳�!*******\n");
		scanf("%d", &cmd_flag);
		switch(cmd_flag)
		{
			case 0:		//��ѯ
				money = DataReadBuf[0]*100 + DataReadBuf[1];
				printf("��ѯ�������:%d\n", money);
				break;
			
			case 1:		//��ֵ
				printf("��������Ҫ��ֵ�Ľ��:\n");
				scanf("%d", &cont);
				money = DataReadBuf[0]*100 + DataReadBuf[1];
				money += cont;		//money = money + cont
				DataWriteBuf[0] = money/100;		//��λ����
				DataWriteBuf[1] = money%100;		//��λ����
				if( PiccWrite(fd , sector) )
				{
					printf("�Բ��𣬳�ֵʧ��!\n");
				}  
				memset(DataReadBuf, 0, sizeof(DataReadBuf));		//��ս��յĻ�����
				printf("�ɹ���ֵ%dԪ, ��ǰ���:%dԪ\n", cont, money);
				break;			
		
			case 2:		//����  --> �Ӽ������룬�����Ѻ������д�뵽RFID��
				break;
			
			case 3:		//�˳�����
				printf("==�˳���ֵ���ܽ���==\n");
				close(fd);
				return 0;		//������ǰ����
			
			default:	//������������
				printf("��������ȷ�Ĳ�����ţ�\n");
				break;
		}
		sleep(1);
	}
	
	close(fd);	
	return 0;
}

/*RFID����:������Ҫ���ѵĽ��*/
int Picc_Consume(int spend_money)
{
	//1, ��ȡ�������
	//1, ��ʼ�� --> �򿪴���, ��������
	int ret, i;
	int fd;
	int cmd_flag;
	int cont;
	int money;		//���
	unsigned char sector;

	fd = open(DEV_PATH, O_RDWR | O_NOCTTY | O_NONBLOCK);		//�򿪴���2  --> con3
	if (fd < 0)
	{
		fprintf(stderr, "Open Gec6818_ttySAC2 fail!\n");
		return -1;
	}
	/*��ʼ������*/
	init_tty(fd);

  	//ѡ������д���ĸ���
	sector = 1;	

	//2, RFID���Ĳ��� --> ���� -->����ײ -->ѡ�� -->��֤��Կ --> ѡ��"��ѯ","��ֵ"
	while(1)
	{
		/*�������߷�Χ�Ŀ�*/
		if ( PiccRequest(fd) )
		{
			sleep(1);
			printf("����ʧ��!\n");
			continue;
		}	
		printf("����ɹ�!\n");
		usleep(30*1000);	
		/*���з���ײ����ȡ���߷�Χ������ID*/
		if( PiccAnticoll(fd) )
		{
			printf("����ײʧ��!\n");
			continue;
		}		
		usleep(30*1000);
		/*����*/
		if( PiccSelect(fd) )
		{
			printf("����ʧ��!\n");
			continue;
		}		
		usleep(30*1000);
		/*������֤ : ��֤��Կ*/
		if( PiccAuthKey( fd,  sector, KEYA))
		{
			printf("4 PiccAuthKey KEYA failed now try KEYB\n");
				
			/*�������߷�Χ�Ŀ�*/
			if ( PiccRequest(fd) )
			{
				printf("5 The request failed!\n");
				continue;
			}
			/*���з���ײ����ȡ���߷�Χ������ID*/
			if( PiccAnticoll(fd) )
			{
				printf("6 RFID PiccAnticoll failed\n");
				continue;
			}
			/*����*/
			if( PiccSelect(fd) )
			{
					printf("7 PiccSelect failed\n");
					continue;
			}
			/*������֤*/
			if( PiccAuthKey( fd,  sector, KEYB) )
				printf("8PiccAuthKey failed\n");
				continue;
		}	
		
		/*��ȡRFID������1������*/
		if( PiccRead(fd , sector) )	
		{
			printf("==��ȡʧ��==\n");
			continue;
		}	
		//2, ���ѽ��
		money = DataReadBuf[0]*100 + DataReadBuf[1];
		printf("��ǰ���:%d Ԫ, �������� %dԪ\n", money, spend_money);
		if(money < spend_money)
		{
			printf("����,����ʧ�ܣ�");
			break;
		}
		money -= spend_money;
		DataWriteBuf[0] = money/100;		//��λ����
		DataWriteBuf[1] = money%100;		//��λ����
		if( PiccWrite(fd , sector) )
		{
			printf("�Բ�������ʧ��!, �������ԣ�\n");
			continue;
		}  
		printf("�ɹ�����%dԪ, ��ǰ���:%dԪ\n", spend_money, money);		
		break;
	}
	close(fd);
	return 0;
}

