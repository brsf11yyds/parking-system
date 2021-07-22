# ifndef BMP_H
# define BMP_H
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <linux/input.h>
#include <termios.h> 
#include <netdb.h>
#include <errno.h>
#include <time.h>
#include "api_v4l2.h"
#include "lcd.h"

#define DEV_PATH   "/dev/ttySAC2"
#define KEYA		0x60
#define KEYB		0x61

#define CAR_IN  0
#define CAR_OUT 1

unsigned char cardid[4];        //存放RFID卡ID
unsigned char DataWriteBuf[16];
unsigned char DataReadBuf[16];
unsigned char DataBfr[16];


struct list_node
{
	int id;   
	unsigned money;
    int state;                     //0里       1外
    int cat;                       //1普通卡    2临时卡
    time_t intial_time;
    char in_picture[20],out_picture[20];
	struct list_node *next;	
};
struct list_node * data0;
/*
BMP格式
这种格式内的数据分为三到四个部分，依次是：
文件信息头 （14字节）存储着文件类型，文件大小等信息
图片信息头 （40字节）存储着图像的尺寸，颜色索引，位平面数等信息
调色板 （由颜色索引数决定）【可以没有此信息】
位图数据 （由图像尺寸决定）每一个像素的信息在这里存储

一般的bmp图像都是24位，也就是真彩。每8位为一字节，24位也就是使用三字节来存储每一个像素的信息，三个字节对应存放r，g，b三原色的数据，
每个字节的存贮范围都是0-255。那么以此类推，32位图即每像素存储r，g，b，a（Alpha通道，存储透明度）四种数据。8位图就是只有灰度这一种信息，
还有二值图，它只有两种颜色，黑或者白。
*/
//文件信息头结构体
typedef struct tagBITMAPFILEHEADER1 
{
    //unsigned short bfType;        // 19778，必须是BM字符串，对应的十六进制为0x4d42,十进制为19778，否则不是bmp格式文件
    unsigned int   bfSize;        // 文件大小 以字节为单位(2-5字节)
    unsigned short bfReserved1;   // 保留，必须设置为0 (6-7字节)
    unsigned short bfReserved2;   // 保留，必须设置为0 (8-9字节)
    unsigned int   bfOffBits;     // 从文件头到像素数据的偏移  (10-13字节)
} BITMAPFILEHEADER1;

//图像信息头结构体
typedef struct tagBITMAPINFOHEADER1 
{
    unsigned int    biSize;          // 此结构体的大小 (14-17字节)
    long            biWidth;         // 图像的宽  (18-21字节)
    long            biHeight;        // 图像的高  (22-25字节)
    unsigned short  biPlanes;        // 表示bmp图片的平面属，显然显示器只有一个平面，所以恒等于1 (26-27字节)
    unsigned short  biBitCount;      // 一像素所占的位数，一般为24   (28-29字节)
    unsigned int    biCompression;   // 说明图象数据压缩的类型，0为不压缩。 (30-33字节)
    unsigned int    biSizeImage;     // 像素数据所占大小, 这个值应该等于上面文件头结构中bfSize-bfOffBits (34-37字节)
    long            biXPelsPerMeter; // 说明水平分辨率，用象素/米表示。一般为0 (38-41字节)
    long            biYPelsPerMeter; // 说明垂直分辨率，用象素/米表示。一般为0 (42-45字节)
    unsigned int    biClrUsed;       // 说明位图实际使用的彩色表中的颜色索引数（设为0的话，则说明使用所有调色板项）。 (46-49字节)
    unsigned int    biClrImportant;  // 说明对图象显示有重要影响的颜色索引的数目，如果是0，表示都重要。(50-53字节)
} BITMAPINFOHEADER1;

//24位图像素信息结构体,即调色板
typedef struct _PixelInfo {
    unsigned char rgbBlue;   //该颜色的蓝色分量  (值范围为0-255)
    unsigned char rgbGreen;  //该颜色的绿色分量  (值范围为0-255)
    unsigned char rgbRed;    //该颜色的红色分量  (值范围为0-255)
    unsigned char rgbReserved;// 保留，必须为0
} PixelInfo;

int   Init_Font(void);  //显示字体库前先调用本函数进行初始化

void  UnInit_Font(void); //程序退出前，调用本函数

int   Clean_Area(int X,  //x坐标起始点
                 int Y,  //y坐标起始点
				 int width, //绘制的宽度
				 int height,//绘制的高度
				 unsigned long color); //往屏幕指定区域填充颜色
				 
                 
int   Display_characterX(unsigned int x,          //x坐标起始点
                         unsigned int y,          //y坐标起始点
						 char *string,   		  //GB2312 中文字符串
						 unsigned int color ,     //字体颜色值
						 int size);               //字体放大倍数 1~8



void showBmpHead(BITMAPFILEHEADER1 pBmpHead);

void showBmpInfoHead(BITMAPINFOHEADER1 pBmpinfoHead);

/*bmp_show.c*/
int show_bmp(char *path);/*显示图片*/
int clearlcd();/*清屏*/
int touch(int *x,int *y);/*触摸返回参数*/

/* rfid.c */
void init_tty(int fd);/* 设置窗口参数:9600速率 */
unsigned char CalBCC(unsigned char *buf, int n);/*计算校验和*/
int PiccRequest(int fd);/*请求*/
int PiccAnticoll(int fd);/*防碰撞，获取范围内最大ID*/
int PiccSelect(int fd);/*选择*/
int PiccHalt(int fd);/*暂停*/
int PiccAuthKey(int fd,int sector,unsigned char KeyAB);/*密码验证*/
int PiccRead(int fd,unsigned char sector);/*读卡*/
int PiccWrite(int fd,unsigned char sector);/*写卡*/
int Picc_Menu(void);/*RFID查询充值*/
int Picc_Consume(int spend_money);/*RFID消费:传入需要消费的金额*/

/*audioplay.c*/
int audioplay(unsigned char *p);/*语言播报*/

/*card.c*/
int card(int select);/*读卡系统*/
int read_card(int* id);
int write_card(int id);

/*time.c*/
int show_time();
int show_pay(int cast);

/*beep_app.c*/
int do_beep();

//data.c
int data();

int find_list_node(struct list_node * head , int id);
int  money_list_node(struct list_node * head ,int id ,unsigned money);
int  id_list_node(struct list_node * head ,int id ,int new_id);
int  state_list_node(struct list_node * head ,int id ,int state);
int  cat_list_node(struct list_node * head ,int id ,int cat);
int  time_list_node(struct list_node * head ,int id ,time_t time);
int  pic_list_node(struct list_node * head ,int id ,char *picture,int inout);
int  state_list_node_2(struct list_node * head ,int id ,int *state);
int  time_list_node_2(struct list_node * head ,int id ,time_t *time);
int  money_list_node_2(struct list_node * head ,int id ,int *money);
int  cat_list_node_2(struct list_node * head ,int id ,int *cat);
int  list_add_head(struct list_node * head ,int id,unsigned money,int cat);
int  delete_list_node(struct list_node * head , int id);

int camera(char* filename);
int syasinn(int id,int inout);
#endif
