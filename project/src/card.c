#include "read.h"
extern unsigned char RBuf[128];
extern struct list_node * data0;



int card(int select)
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
	DataWriteBuf[0]  = '3';
	DataWriteBuf[1]  = '2';
	DataWriteBuf[2]  = '1';
	DataWriteBuf[3]  = '4';
	DataWriteBuf[4]  = '5';
	DataWriteBuf[5]  = '6';
	DataWriteBuf[6]  = '7';
	DataWriteBuf[7]  = '8';
	DataWriteBuf[8]  = '9';
	DataWriteBuf[9]  = '2';
	DataWriteBuf[10] = '5';
	DataWriteBuf[11] = '6';
	DataWriteBuf[12] = '7';
	DataWriteBuf[13] = '3';
	DataWriteBuf[14] = '3';
	DataWriteBuf[15] = '3';

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
			if(select==1)//若为临时卡，写入数据块
			{
				/*判断是否在数据库内*/
				/*获取卡的数据块1的数据*/
				if( PiccRead(fd , sector) )	
				{
					printf("==PiccRead failed\n");
					continue;
				}
				printf("1DataReadBuf= %s\n",DataReadBuf);
				memset(DataReadBuf,0,16);
				/*如果是空卡*/
				if(strcmp(DataReadBuf,"0000000000000000") == 0)
				{
					
				}
				/*如果不是空卡*/
				else
				{

				}
				/*写入数据块1*/
				if( PiccWrite(fd , sector) )
				{
					printf("PiccWrite failed\n");
					//  continue;
				}  
				memset(DataReadBuf,0,16);	//清空 数据缓冲区
				memset(cardid,0,4);			//请空 卡号
				usleep(300*1000);
			} 
			else if(select==2)
			{
				usleep(300*1000);
			}//若为普通卡，不进行写入

			/*获取卡的数据块1的数据*/
			if( PiccRead(fd , sector) )	
			{
				printf("==PiccRead failed\n");
				continue;
			}
			printf("1DataReadBuf= %s\n",DataReadBuf);
			memset(DataReadBuf,0,16);
			
			
			sleep(2);		
			// if( PiccRead(fd , sector+1) )		//读取扇区2的内容
			// {
			// 	printf("==PiccRead failed\n");
			// 	continue;
			// }
			// printf("2DataReadBuf= %s\n",DataReadBuf);

			// memset(DataReadBuf,0,16);
			// memset(cardid,0,4);
			// usleep(300*1000);      
		}

        memset(DataReadBuf,0,16);
        memset(cardid,0,4);
        sleep(1);
			
	}   
	close(fd);
	return 0;	
}
