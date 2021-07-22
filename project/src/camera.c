#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <linux/input.h>
#include <unistd.h>
#include <pthread.h>
#include <linux/videodev2.h>
#include "api_v4l2.h"
#include "lcd.h"
#include "read.h"


static FrameBuffer camera_frame;

int syasinn(int id,int inout)
{
	char filename[20];
	if(inout == CAR_IN)
	{
		sprintf(filename,"save/%d_in.jpg",id);
		pic_list_node(data0,id,filename,inout);
	}
	else
	{
		sprintf(filename,"save/%d_out.jpg",id);
		pic_list_node(data0,id,filename,inout);
		
	}
	camera(filename);
}

int camera(char* filename)
{
	unsigned int 	i=0;
	
	/* 打开LCD设备 */
	//lcd_open();
	
	/* 显示camera.jpg */
	//lcd_draw_jpg_file(0,0,"camera.jpg");
	
	/* 打开摄像头设备 
	   开发板复位的时候不能连接摄像头，否则会打开出错！
	   正确的方法：等开发板复位成功之后，才接入摄像头
	*/
	linux_v4l2_device_init("/dev/video7",640,480);
	
	/* 启动摄像头捕捉 */	
	linux_v4l2_start_capturing();
	

	/* 获取摄像头的数据 */
	linux_v4l2_get_frame(&camera_frame);

	/* 显示摄像头的数据 */
	//lcd_draw_camera(0,0,camera_frame);		
		

	/* 抓拍一张图片 */
	/* 创建jpg图片 */
	linux_v4l2_save_image_file(filename,camera_frame);	
			
	/* 关闭LCD设备 */
	//lcd_close();
	
	return 0;	
}

