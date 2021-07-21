#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


int init_tty(int fd)
{
	struct termios old_flags,new_flags;
	bzero(&new_flags,sizeof(new_flags));
	
	//1. 获取旧的属性
	tcgetattr(fd,&old_flags);
	
	//2. 设置原始模式
	cfmakeraw(&new_flags);
	
	//3. 激活本地连接CLOCAL与接收使能CREAD的选项
	new_flags.c_cflag |= CLOCAL | CREAD; 
	
	//4. 设置波特率
	cfsetispeed(&new_flags, B9600); 
	cfsetospeed(&new_flags, B9600);
	
	//5. 设置数据位为8位
	new_flags.c_cflag &= ~CSIZE; //清空原有的数据位
	new_flags.c_cflag |= CS8;
	
	//6. 设置奇偶检验位
	new_flags.c_cflag &= ~PARENB;
	
	//7. 设置一位停止位
	new_flags.c_cflag &= ~CSTOPB;
	
	//8. 设置等待时间，最少接收字符个数
	new_flags.c_cc[VTIME] = 0;
	new_flags.c_cc[VMIN] = 1;
	
	//9. 清空串口缓冲区
	tcflush(fd, TCIFLUSH);
	
	//10. 设置串口的属性到文件中
	tcsetattr(fd, TCSANOW, &new_flags);
	
	return 0;
}

char get_bcc(char *buf,int n)
{
	char bcc = 0;
	int i;
	for(i=0;i<n;i++)
	{
		bcc ^= buf[i];
	}
	
	return (~bcc);
}

int main()
{
	//1. 访问串口2
	int fd = open("/dev/ttySAC2",O_RDWR | O_NOCTTY);
	
	//2. 配置串口2参数
	init_tty(fd);
	
	//3. 往RFID模块中发送请求命令字
	char wbuf[7];
	char kbuf[8];
	
	//防碰撞的应答帧是0x0A 
	char qbuf[65];
	
	bzero(wbuf,7);
	wbuf[0] = 0x07; //帧长=7byte
	wbuf[1] = 0x02; //命令类型
	wbuf[2] = 'A';  //命令'A' 代表请求
	wbuf[3] = 0x01; //信息长度
	wbuf[4] = 0x52; //信息  ALL=0x52
	wbuf[5] = get_bcc(wbuf,wbuf[0]-2); //计算校验和 
	wbuf[6] = 0x03; //结束标志
	
	while(1)
	{
		//不断发送数据给RFID，使得RFID模块不断请求附近有没有卡
		//往串口中发送数据
		tcflush(fd,TCIFLUSH);
		write(fd,wbuf,strlen(wbuf));
		
		//等待数据全部写入RFID中
		usleep(10000);
		
		bzero(qbuf,8);
		read(fd,qbuf,sizeof(qbuf));
		
		//如果状态为0，则说明有卡在附近
		if(qbuf[2] == 0x00)
		{
			printf("get card ok!\n");
			break;
		}
	}
	
	//确保卡在附近，发送防碰撞命令字给RFID模块
	int cardid;
	
	bzero(kbuf,8);
	kbuf[0] = 0x08;  //帧长 8byte
	kbuf[1] = 0x02;  //命令类型ISO14443A命令 0x02
	kbuf[2] = 'B';    //命令：防碰撞
	kbuf[3] = 0x02;  //信息长度
	kbuf[4] = 0x93;  //一级防碰撞
	kbuf[5] = 0x00;  //位计数
	kbuf[6] = get_bcc(kbuf,kbuf[0]-2); //校验和
	kbuf[7] = 0x03;  //结束标志
	
	printf("111111111111\n");
	
	//往串口中发送数据
	tcflush(fd,TCIFLUSH);
	
	printf("%d\n",strlen(kbuf)); //5
	write(fd,kbuf,8);
		
	//等待数据全部写入RFID中
	usleep(10000);
		
	bzero(qbuf,65);
	read(fd,qbuf,65);
	
	printf("222222222222\n");
	
	if(qbuf[2] == 0x00)  //能够获取到卡号
	{
		int i,j;
		for(i=3,j=0;i>=0;i--,j++)
		{
			memcpy((char *)&cardid+j,&qbuf[4+i],1);
		}	
		printf("get cardid ok!\n");
	}
	else{
		printf("get cardid error!\n");
		
	}
	
	printf("cardid = %#x\n",cardid);   //#：代表带着格式输出
	
	close(fd);
	
	return 0;
}