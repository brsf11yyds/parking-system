#ifndef _RFID_H_
#define _RFID_H_

#include <stdio.h>
#include <fcntl.h> 
#include <unistd.h>
#include <termios.h> 
#include <sys/types.h>
#include <sys/select.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <time.h>


#define DEV_PATH   "/dev/ttySAC2"
#define KEYA		0x60
#define KEYB		0x61

unsigned char cardid[4];        //存放RFID卡ID
unsigned char DataWriteBuf[16];
unsigned char DataReadBuf[16];
unsigned char DataBfr[16];

/* 设置窗口参数:9600速率 */
void init_tty(int fd);
/*计算校验和*/
unsigned char CalBCC(unsigned char *buf, int n);
/*请求*/
int PiccRequest(int fd);
/*防碰撞，获取范围内最大ID*/
int PiccAnticoll(int fd);
/*选择*/
int PiccSelect(int fd);
/*暂停*/
int PiccHalt(int fd);
/*密码验证*/
int PiccAuthKey(int fd,int sector,unsigned char KeyAB);
/*读卡*/
int PiccRead(int fd,unsigned char sector);
/*写卡*/
int PiccWrite(int fd,unsigned char sector);

/*RFID查询充值*/
int Picc_Menu(void);
/*RFID消费:传入需要消费的金额*/
int Picc_Consume(int spend_money);

int show_bmp(char *bmp_name);

int music(unsigned char *p);

#endif 
