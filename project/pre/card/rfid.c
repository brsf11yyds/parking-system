#include "rfid.h"
unsigned char RBuf[128];
unsigned char KEYA_BUF[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
unsigned char KEYB_BUF[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

/* 设置窗口参数:9600速率 */
void init_tty(int fd)
{    
	//声明设置串口的结构体
	struct termios termios_new;
	//先清空该结构体
	bzero( &termios_new, sizeof(termios_new));
	//	cfmakeraw()设置终端属性，就是设置termios结构中的各个参数。
	cfmakeraw(&termios_new);
	//设置波特率
	//termios_new.c_cflag=(B9600);
	cfsetispeed(&termios_new, B9600);
	cfsetospeed(&termios_new, B9600);
	//CLOCAL和CREAD分别用于本地连接和接受使能，因此，首先要通过位掩码的方式激活这两个选项。    
	termios_new.c_cflag |= CLOCAL | CREAD;
	//通过掩码设置数据位为8位
	termios_new.c_cflag &= ~CSIZE;
	termios_new.c_cflag |= CS8; 
	//设置无奇偶校验
	termios_new.c_cflag &= ~PARENB;
	//一位停止位
	termios_new.c_cflag &= ~CSTOPB;
	tcflush(fd,TCIFLUSH);
	// 可设置接收字符和等待时间，无特殊要求可以将其设置为0
	termios_new.c_cc[VTIME] = 10;
	termios_new.c_cc[VMIN] = 1;
	// 用于清空输入/输出缓冲区
	tcflush (fd, TCIFLUSH);
	//完成配置后，可以使用以下函数激活串口设置
	if(tcsetattr(fd,TCSANOW,&termios_new) )
		printf("Setting the serial1 failed!\n");

}

/*计算校验和*/
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

/*请求*/
int PiccRequest(int fd)
{
	unsigned char WBuf[128], RBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x07;	//帧长= 7 Byte
	WBuf[1] = 0x82;	//包号= 0 , 命令类型= 0x02
	WBuf[2] = 'A';	//命令= 'A'
	WBuf[3] = 0x01;	//信息长度= 1
	WBuf[4] = 0x52;	//请求模式:  ALL=0x52
	WBuf[5] = CalBCC(WBuf, WBuf[0]-2);		//校验和
	WBuf[6] = 0x03; 	//结束标志

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
			if (RBuf[2] == 0x00)	 	//应答帧状态部分为0 则请求成功  返回卡的类型
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

/*防碰撞，获取范围内最大ID*/
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
	WBuf[0] = 0x0C;	//帧长= 8 Byte
	WBuf[1] = 0xC2;	//包号= 0 , 命令类型= 0x02
	WBuf[2] = 'B';	//命令= 'B'
	WBuf[3] = 0x06;	//信息长度= 2
	WBuf[4] = 0x93;	//防碰撞0x93 --一级防碰撞
	WBuf[5] = 0x0A;	//位计数0
	WBuf[6] = 0xE2;
	WBuf[7] = 0xB5;
	WBuf[8] = 0x5D;
	WBuf[9] = 0xBB;
	WBuf[10] = CalBCC(WBuf, WBuf[0]-2);		//校验和
	WBuf[11] = 0x03; 	//结束标志
#endif

#if 1
	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x08;	//帧长= 8 Byte
	WBuf[1] = 0x02;	//包号= 0 , 命令类型= 0x02
	WBuf[2] = 'B';	//命令= 'B'
	WBuf[3] = 0x02;	//信息长度= 2
	WBuf[4] = 0x93;	//防碰撞0x93 --一级防碰撞
	WBuf[5] = 0x00;	//位计数0
	WBuf[6] = CalBCC(WBuf, WBuf[0]-2);		//校验和
	WBuf[7] = 0x03; 	//结束标志
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
			if (RBuf[2] == 0x00) //应答帧状态部分为0 则获取ID 成功
			{
				ID = (RBuf[7]<<24) | (RBuf[6]<<16) | (RBuf[5]<<8) | RBuf[4];
				
				memcpy(cardid,&RBuf[4],4);
				printf("PiccAnticoll The card ID is %x\n",ID);
				return 0;
			}
	}
	return -1;
}

/*选择*/
int PiccSelect(int fd)
{
	unsigned char WBuf[128], RBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x0B;	//帧长= 7 Byte
	WBuf[1] = 0x02;	//包号= 0 , 命令类型= 0x02
	WBuf[2] = 'C';	//命令= 'C
	WBuf[3] = 0x05;	//信息长度= 5
	WBuf[4] = 0x93;	//请求模式:  ALL=0x52
        memcpy(&WBuf[5],cardid,4);
	WBuf[9] = CalBCC(WBuf, WBuf[0]-2);		//校验和
	WBuf[10] = 0x03; 	//结束标志

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
			if (RBuf[2] == 0x00)	 	//应答帧状态部分为0 则请求成功
			{
			        printf("PiccSelect success\n");
				return 0;
			}
			break;
	}
	return -1;
}

/*暂停*/
int PiccHalt(int fd)
{
	unsigned char WBuf[128], RBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x06;	//帧长= 6 Byte
	WBuf[1] = 0x02;	//包号= 0 , 命令类型= 0x02
	WBuf[2] = 'D';	//命令= 'D
	WBuf[3] = 0x00;	//信息长度= 0
	WBuf[4] = CalBCC(WBuf, WBuf[0]-2);		//校验和
	WBuf[5] = 0x03; 	//结束标志

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
			if (RBuf[2] == 0x00)	 	//应答帧状态部分为0 则请求成功
			{
			        printf("PiccHalt success\n");
				return 0;
			}
			break;
	}
	return -1;
}

/*密码验证*/
int PiccAuthKey(int fd,int sector,unsigned char KeyAB)
{
	unsigned char WBuf[128], RBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x12;	//帧长= 17 Byte
	WBuf[1] = 0x02;	//包号= 0 , 命令类型= 0x02
	WBuf[2] = 'F';	//命令= 'F'
	WBuf[3] = 12;	//信息长度= 0x0c
	WBuf[4] = KeyAB;
        memcpy(&WBuf[5],cardid,4);
        memcpy(&WBuf[9],KEYA_BUF,6);
        WBuf[15]=sector;   //读写哪个区
	WBuf[16] = CalBCC(WBuf, WBuf[0]-2);		//校验和
	WBuf[17] = 0x03; 	//结束标志

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
			if (RBuf[2] == 0x00)	 	//应答帧状态部分为0 则请求成功
			{
			        printf("PiccAuthKey success\n");
				return 0;
			}
			break;
	}
	return -1;
}

/*读卡*/
int PiccRead(int fd,unsigned char sector)
{
	unsigned char WBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 0x07;	//帧长= 7 Byte
	WBuf[1] = 0x02;	//包号= 0 , 命令类型= 0x02
	WBuf[2] = 'G';	//命令= 'g
	WBuf[3] = 0x01;	//信息长度= 1
	WBuf[4] = sector;	//读哪个扇区
	WBuf[5] = CalBCC(WBuf, WBuf[0]-2);		//校验和
	WBuf[6] = 0x03; 	//结束标志

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
			if (RBuf[2] == 0x00)	 	//应答帧状态部分为0 则请求成功
			{
			        usleep(10*1000);
			        read(fd,&RBuf[ret],6);
			        memcpy(DataReadBuf,&RBuf[4],16); //读取到该扇区的数据
			        for(i=0;i<23;i++)
			            printf("PiccRead RBuf[%d]=%x\n",i,RBuf[i]);
				return 0;
			}
			break;
	}
	return -1;
}

/*写卡*/
int PiccWrite(int fd,unsigned char sector)
{
	unsigned char WBuf[128], RBuf[128];
	int  ret, i;
	fd_set rdfd;
	static struct timeval timeout;

	memset(WBuf, 0, 128);
	memset(RBuf,0,128);
	WBuf[0] = 23;	//帧长= 23 Byte
	WBuf[1] = 0x02;	//包号= 0 , 命令类型= 0x02
	WBuf[2] = 'H';	//命令= 'H
	WBuf[3] = 0X11;	//信息长度= 17
	WBuf[4] = sector;   //写哪个扇区
        memcpy(&WBuf[5],DataWriteBuf,16);
	WBuf[21] = CalBCC(WBuf, WBuf[0]-2);		//校验和
	WBuf[22] = 0x03; 	//结束标志

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
			if (RBuf[2] == 0x00)	 	//应答帧状态部分为0 则请求成功
			{
			        printf("PiccWrite card success\n");
				return 0;
			}
			break;
	}
	return -1;
}



/*RFID查询充值*/
int Picc_Menu(void)
{
	//1, 初始化 --> 打开串口, 串口配置
	int ret, i;
	int fd;
	int cmd_flag;
	int cont;
	int money;		//金额
	unsigned char sector;

	fd = open(DEV_PATH, O_RDWR | O_NOCTTY | O_NONBLOCK);		//打开串口2  --> con3
	if (fd < 0)
	{
		fprintf(stderr, "Open Gec6818_ttySAC2 fail!\n");
		return -1;
	}
	/*初始化串口*/
	init_tty(fd);

  	//选择将数据写到哪个块
	sector = 1;	

	//2, RFID卡的操作 --> 请求 -->防碰撞 -->选择 -->验证密钥 --> 选择"查询","充值"
	while(1)
	{
		/*请求天线范围的卡*/
		if ( PiccRequest(fd) )
		{
			sleep(1);
			printf("请求失败!\n");
			continue;
		}	
		printf("请求成功!\n");
		usleep(30*1000);	
		/*进行防碰撞，获取天线范围内最大的ID*/
		if( PiccAnticoll(fd) )
		{
			printf("防碰撞失败!\n");
			continue;
		}		
		usleep(30*1000);
		/*设置*/
		if( PiccSelect(fd) )
		{
			printf("设置失败!\n");
			continue;
		}		
		usleep(30*1000);
		/*密码验证 : 验证密钥*/
		if( PiccAuthKey( fd,  sector, KEYA))
		{
			printf("4 PiccAuthKey KEYA failed now try KEYB\n");
				
			/*请求天线范围的卡*/
			if ( PiccRequest(fd) )
			{
				printf("5 The request failed!\n");
				continue;
			}
			/*进行防碰撞，获取天线范围内最大的ID*/
			if( PiccAnticoll(fd) )
			{
				printf("6 RFID PiccAnticoll failed\n");
				continue;
			}
			/*设置*/
			if( PiccSelect(fd) )
			{
					printf("7 PiccSelect failed\n");
					continue;
			}
			/*密码验证*/
			if( PiccAuthKey( fd,  sector, KEYB) )
				printf("8PiccAuthKey failed\n");
				continue;
		}	
		
		/*读取RFID卡扇区1的数据*/
		if( PiccRead(fd , sector) )	
		{
			printf("==读取失败==\n");
			continue;
		}
		printf("\n*******选择需要执行的操作: 0 查询; 1 充值; 2 消费; 3 退出!*******\n");
		scanf("%d", &cmd_flag);
		switch(cmd_flag)
		{
			case 0:		//查询
				money = DataReadBuf[0]*100 + DataReadBuf[1];
				printf("查询到的余额:%d\n", money);
				break;
			
			case 1:		//充值
				printf("请输入需要充值的金额:\n");
				scanf("%d", &cont);
				money = DataReadBuf[0]*100 + DataReadBuf[1];
				money += cont;		//money = money + cont
				DataWriteBuf[0] = money/100;		//高位数据
				DataWriteBuf[1] = money%100;		//低位数据
				if( PiccWrite(fd , sector) )
				{
					printf("对不起，充值失败!\n");
				}  
				memset(DataReadBuf, 0, sizeof(DataReadBuf));		//清空接收的缓冲区
				printf("成功充值%d元, 当前余额:%d元\n", cont, money);
				break;			
		
			case 2:		//消费  --> 从键盘输入，把消费后的数据写入到RFID卡
				break;
			
			case 3:		//退出功能
				printf("==退出充值功能界面==\n");
				close(fd);
				return 0;		//结束当前函数
			
			default:	//操作命令有误
				printf("请输入正确的操作编号！\n");
				break;
		}
		sleep(1);
	}
	
	close(fd);	
	return 0;
}

/*RFID消费:传入需要消费的金额*/
int Picc_Consume(int spend_money)
{
	//1, 读取卡中余额
	//1, 初始化 --> 打开串口, 串口配置
	int ret, i;
	int fd;
	int cmd_flag;
	int cont;
	int money;		//金额
	unsigned char sector;

	fd = open(DEV_PATH, O_RDWR | O_NOCTTY | O_NONBLOCK);		//打开串口2  --> con3
	if (fd < 0)
	{
		fprintf(stderr, "Open Gec6818_ttySAC2 fail!\n");
		return -1;
	}
	/*初始化串口*/
	init_tty(fd);

  	//选择将数据写到哪个块
	sector = 1;	

	//2, RFID卡的操作 --> 请求 -->防碰撞 -->选择 -->验证密钥 --> 选择"查询","充值"
	while(1)
	{
		/*请求天线范围的卡*/
		if ( PiccRequest(fd) )
		{
			sleep(1);
			printf("请求失败!\n");
			continue;
		}	
		printf("请求成功!\n");
		usleep(30*1000);	
		/*进行防碰撞，获取天线范围内最大的ID*/
		if( PiccAnticoll(fd) )
		{
			printf("防碰撞失败!\n");
			continue;
		}		
		usleep(30*1000);
		/*设置*/
		if( PiccSelect(fd) )
		{
			printf("设置失败!\n");
			continue;
		}		
		usleep(30*1000);
		/*密码验证 : 验证密钥*/
		if( PiccAuthKey( fd,  sector, KEYA))
		{
			printf("4 PiccAuthKey KEYA failed now try KEYB\n");
				
			/*请求天线范围的卡*/
			if ( PiccRequest(fd) )
			{
				printf("5 The request failed!\n");
				continue;
			}
			/*进行防碰撞，获取天线范围内最大的ID*/
			if( PiccAnticoll(fd) )
			{
				printf("6 RFID PiccAnticoll failed\n");
				continue;
			}
			/*设置*/
			if( PiccSelect(fd) )
			{
					printf("7 PiccSelect failed\n");
					continue;
			}
			/*密码验证*/
			if( PiccAuthKey( fd,  sector, KEYB) )
				printf("8PiccAuthKey failed\n");
				continue;
		}	
		
		/*读取RFID卡扇区1的数据*/
		if( PiccRead(fd , sector) )	
		{
			printf("==读取失败==\n");
			continue;
		}	
		//2, 消费金额
		money = DataReadBuf[0]*100 + DataReadBuf[1];
		printf("当前余额:%d 元, 本次消费 %d元\n", money, spend_money);
		if(money < spend_money)
		{
			printf("余额不足,消费失败！");
			break;
		}
		money -= spend_money;
		DataWriteBuf[0] = money/100;		//高位数据
		DataWriteBuf[1] = money%100;		//低位数据
		if( PiccWrite(fd , sector) )
		{
			printf("对不起，消费失败!, 正在重试！\n");
			continue;
		}  
		printf("成功消费%d元, 当前余额:%d元\n", spend_money, money);		
		break;
	}
	close(fd);
	return 0;
}

