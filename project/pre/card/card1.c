#include "rfid.h"
extern unsigned char RBuf[128];
int main()
{
	int ret, i;
	int fd;
	unsigned char sector;

	fd = open(DEV_PATH, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd < 0)
	{
		fprintf(stderr, "Open Gec6818_ttySAC2 fail!\n");
		return -1;
	}
	/*初始化串口*/
	init_tty(fd);

  	//选择将数据写到哪个块
	sector = 1;

	//待写入的数据  ==> 16个字节 ==> 写入其中一个扇区
	DataWriteBuf[0]  = 0x33;
	DataWriteBuf[1]  = 0x11;
	DataWriteBuf[2]  = 0x41;
	DataWriteBuf[3]  = 0x41;
	DataWriteBuf[4]  = 0x41;
	DataWriteBuf[5]  = 0x55;
	DataWriteBuf[6]  = 0x66;
	DataWriteBuf[7]  = 0x77;
	DataWriteBuf[8]  = 0x88;
	DataWriteBuf[9]  = 0x99;
	DataWriteBuf[10] = 0xaa;
	DataWriteBuf[11] = 0xbb;
	DataWriteBuf[12] = 0xcc;
	DataWriteBuf[13] = 0xdd;
	DataWriteBuf[14] = 0x11;
	DataWriteBuf[15] = 0x88;

	while(1)
	{
		printf("Start..............\n");
		/*请求天线范围的卡*/
		if ( PiccRequest(fd) )
		{
			sleep(2);
			printf("1 The request failed!\n");
				continue;
		}
		usleep(30*1000);		//延时30ms
		/*进行防碰撞，获取天线范围内最大的ID*/
		if( PiccAnticoll(fd) )
		{
			printf("2 RFID PiccAnticoll failed\n");
			continue;
		}

		usleep(30*1000);
		//选择
		if( PiccSelect(fd) )
		{
				printf("3 PiccSelect failed\n");
				continue;
		}

		usleep(30*1000);
		//密码验证 : 验证秘钥
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

        usleep(300*1000);
		i=2;
		while(i--)
		{        
			///*写入数据块1*/
			//if( PiccWrite(fd , sector) )
			//{
			//	printf("PiccWrite failed\n");
			////  continue;
			//}  
			//memset(DataReadBuf,0,16);	//清空 数据缓冲区
			//memset(cardid,0,4);			//请空 卡号
			
			usleep(300*1000);
			/*获取卡的数据块1的数据*/
			if( PiccRead(fd , sector) )	
			{
				printf("==PiccRead failed\n");
				continue;
			}
			printf("1DataReadBuf= %s\n",DataReadBuf);
			if(RBuf[4]  == 0x33 ){show_bmp("1.bmp");}
			if(RBuf[4]  == 0x00 ) {show_bmp("2.bmp");}

			memset(DataReadBuf,0,16);
			
			
			sleep(2);		
			if( PiccRead(fd , sector+1) )		//读取扇区2的内容
			{
				printf("==PiccRead failed\n");
				continue;
			}
			printf("2DataReadBuf= %s\n",DataReadBuf);
			
			memset(DataReadBuf,0,16);
			memset(cardid,0,4);
			usleep(300*1000);      
		}

        memset(DataReadBuf,0,16);
        memset(cardid,0,4);
        sleep(1);
			
	}   
	close(fd);
	return 0;	
}
