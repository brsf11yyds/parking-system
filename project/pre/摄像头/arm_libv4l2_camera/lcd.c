/****************************************************************************************
 *�ļ�����:lcd.c
 *��    ��:2015-5-29
 *˵	��:��ʾjpg�ļ�
 ----------------------------------------------------------------------------------------
 *�޸�����:2018-6-25
 *˵	��:��ʾjpg��bmp�ļ���jpg��bmp���ݡ���rgb���ݡ�
 ----------------------------------------------------------------------------------------
 *�޸�����:2018-7-3
 *˵	��:�����lcd_draw_camera�������Զ�ʶ��yuyv��ʽ��jpg��ʽ����ͷ
			����ʾ��
****************************************************************************************/
#include <stdio.h>   	//printf scanf
#include <fcntl.h>		//open write read lseek close  	 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <linux/videodev2.h>

#include "common.h"
#include "api_v4l2.h"
#include "lcd.h"
#include "jpeglib.h"


static unsigned char g_color_buf[FB_SIZE]={0};

static int  g_fb_fd;
static int *g_pfb_memory;


//��ʼ��LCD
int lcd_open(void)
{
	g_fb_fd = open("/dev/fb0", O_RDWR);
	
	if(g_fb_fd<0)
	{
			printf("open lcd error\n");
			return -1;
	}

	g_pfb_memory  = (int *)mmap(	NULL, 					//ӳ�����Ŀ�ʼ��ַ������ΪNULLʱ��ʾ��ϵͳ����ӳ��������ʼ��ַ
									FB_SIZE, 				//ӳ�����ĳ���
									PROT_READ|PROT_WRITE, 	//���ݿ��Ա���ȡ��д��
									MAP_SHARED,				//�����ڴ�
									g_fb_fd, 				//��Ч���ļ�������
									0						//��ӳ��������ݵ����
								);

	return g_fb_fd;

}

//LCD����
void lcd_draw_point(unsigned int x,unsigned int y, unsigned int color)
{
	*(g_pfb_memory+y*800+x)=color;
}

//BMP����
int lcd_draw_bmp(unsigned int x,unsigned int y,unsigned char *bmp_buf,unsigned int bmp_buf_size)   
{
	unsigned int blue, green, red;
	unsigned int color;
	unsigned int bmp_width;
	unsigned int bmp_height;
	unsigned int bmp_type;
	unsigned int bmp_size;
	unsigned int x_s = x;
	unsigned int x_e ;	
	unsigned int y_e ;
	unsigned char buf[54]={0};
	unsigned char *pbmp_buf;		 
	
	memcpy(buf,bmp_buf,54);
	
	/* ���  */
	bmp_width =buf[18];
	bmp_width|=buf[19]<<8;
	//printf("bmp_width=%d\r\n",bmp_width);
	
	/* �߶�  */
	bmp_height =buf[22];
	bmp_height|=buf[23]<<8;
	//printf("bmp_height=%d\r\n",bmp_height);	
	
	/* �ļ����� */
	bmp_type =buf[28];
	bmp_type|=buf[29]<<8;
	//printf("bmp_type=%d\r\n",bmp_type);	

	/* ������ʾx��y�������λ�� */
	x_e = x + bmp_width;
	y_e = y + bmp_height;
	
	
	pbmp_buf = &bmp_buf[54];
	
	for(;y < y_e; y++)
	{
		for (;x < x_e; x++)
		{
			/* ��ȡ��������ɫ���� */
			blue  = *pbmp_buf++;
			green = *pbmp_buf++;
			red   = *pbmp_buf++;
			
			/* �жϵ�ǰ��λͼ�Ƿ�32λ��ɫ */
			if(bmp_type == 32)
			{
				pbmp_buf++;
			}
			
			/* ���24bit��ɫ */
			color = red << 16 | green << 8 | blue << 0;
			lcd_draw_point(x, y, color);				
		}
		
		x = x_s;
	}			

	
	return 0;
}

//LCD�����ַ����ͼƬ
int lcd_draw_bmp_file(unsigned int x,unsigned int y,const char *pbmp_path)   
{
			 int bmp_fd;
	unsigned int blue, green, red;
	unsigned int color;
	unsigned int bmp_width;
	unsigned int bmp_height;
	unsigned int bmp_type;
	unsigned int bmp_size;
	unsigned int x_s = x;
	unsigned int y_s = y;
	unsigned int x_e ;	
	unsigned int y_e ;
	unsigned char buf[54]={0};
	unsigned char *pbmp_buf=g_color_buf;
	unsigned char *tmp_buf=NULL;
	
	/* ����λͼ��Դ��Ȩ�޿ɶ���д */	
	bmp_fd=open(pbmp_path,O_RDWR);
	
	if(bmp_fd == -1)
	{
	   printf("open bmp error\r\n");
	   
	   return -1;	
	}
	
	/* ��ȡλͼͷ����Ϣ */
	read(bmp_fd,buf,54);
	
	/* ���  */
	bmp_width =buf[18];
	bmp_width|=buf[19]<<8;
	//printf("bmp_width=%d\r\n",bmp_width);
	
	/* �߶�  */
	bmp_height =buf[22];
	bmp_height|=buf[23]<<8;
	//printf("bmp_height=%d\r\n",bmp_height);	
	
	/* �ļ����� */
	bmp_type =buf[28];
	bmp_type|=buf[29]<<8;
	//printf("bmp_type=%d\r\n",bmp_type);	

	/* ������ʾx��y�������λ�� */
	x_e = x + bmp_width;
	y_e = y + bmp_height;
	
	/* ��ȡλͼ�ļ��Ĵ�С */
	bmp_size=file_size_get(pbmp_path);
	
	/* ��ȡ����RGB���� */
	read(bmp_fd,pbmp_buf,bmp_size-54);
	
	//�����ڴ�ռ����ڴ洢��Y�᾵��ת��λͼ����
	tmp_buf=(char *)calloc(1,bmp_size);
	
	//��ͼƬ��ת,�ؼ���Y�᾵��ת��ͬʱÿ�����ص�ռ��3���ֽ�
	for(y=0;y<bmp_height;y++)
	{
		for(x=0;x<bmp_width;x++)
		{
			*(tmp_buf+(bmp_width*(bmp_height-1-y)+x)*3)		=*(pbmp_buf+(bmp_width*y+x)*3);
			*(tmp_buf+(bmp_width*(bmp_height-1-y)+x)*3+1) 	=*(pbmp_buf+(bmp_width*y+x)*3+1);
			*(tmp_buf+(bmp_width*(bmp_height)+x)*3+2) 		=*(pbmp_buf+(bmp_width)*3+2);
		}
	}	
	
	x=x_s;
	y=y_s;
	
	for(;y < y_e; y++)
	{
		for (;x < x_e; x++)
		{
			/* ��ȡ��������ɫ���� */
			blue  = *pbmp_buf++;
			green = *pbmp_buf++;
			red   = *pbmp_buf++;
			
			/* �жϵ�ǰ��λͼ�Ƿ�32λ��ɫ */
			if(bmp_type == 32)
			{
				pbmp_buf++;
			}
			
			/* ���24bit��ɫ */
			color = red << 16 | green << 8 | blue << 0;
			lcd_draw_point(x, y, color);				
		}
		
		x = x_s;
	}
	
	//�ͷ��ڴ�
	free(tmp_buf);
	
	/* ����ʹ��BMP�����ͷ�bmp��Դ */
	close(bmp_fd);	
	
	return 0;
}


//jpg����
int lcd_draw_jpg(unsigned int x,unsigned int y,char *pjpg_buf,unsigned int jpg_buf_size)  
{
	/*���������󣬴��������*/
	struct 	jpeg_decompress_struct 	cinfo;
	struct 	jpeg_error_mgr 			jerr;	
	
	unsigned char 	*pcolor_buf = g_color_buf;
	char 	*pjpg;
	
	unsigned int 	i=0;
	unsigned int	color =0;
	unsigned int	count =0;
	
	unsigned int 	x_s = x;
	unsigned int 	x_e ;	
	unsigned int 	y_e ;
	
	unsigned int 	jpg_size;
	
	unsigned int 	jpg_width;
	unsigned int 	jpg_height;
	


	jpg_size = jpg_buf_size;
		
	pjpg = pjpg_buf;


	/*ע�������*/
	cinfo.err = jpeg_std_error(&jerr);

	/*��������*/
	jpeg_create_decompress(&cinfo);

	/*ֱ�ӽ����ڴ�����*/		
	jpeg_mem_src(&cinfo,pjpg,jpg_size);
	
	/*���ļ�ͷ*/
	jpeg_read_header(&cinfo, TRUE);

	/*��ʼ����*/
	jpeg_start_decompress(&cinfo);	
	
	x_e	= x_s+cinfo.output_width;
	y_e	= y  +cinfo.output_height;	

	/*����������*/
	while(cinfo.output_scanline < cinfo.output_height )
	{		
		pcolor_buf = g_color_buf;
		
		/* ��ȡjpgһ�е�rgbֵ */
		jpeg_read_scanlines(&cinfo,(JSAMPARRAY)&pcolor_buf,1);
		
		for(i=0; i<cinfo.output_width; i++)
		{
			/* ��ȡrgbֵ */
			color = 		*(pcolor_buf+2);
			color = color | *(pcolor_buf+1)<<8;
			color = color | *(pcolor_buf)<<16;
			
			/* ��ʾ���ص� */
			lcd_draw_point(x,y,color);
			
			pcolor_buf +=3;
			
			x++;
		}
		
		/* ���� */
		y++;			
		
		x = x_s;	
	}		
				
	/* ������� */
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	
	return 0;
}


int lcd_draw_jpg_file(unsigned int x,unsigned int y,const char *pjpg_path)  
{
	/*���������󣬴��������*/
	struct 	jpeg_decompress_struct 	cinfo;
	struct 	jpeg_error_mgr 			jerr;	
	
	unsigned char 	*pcolor_buf = g_color_buf;
	char 			*pjpg;
	
	unsigned int 	i=0;
	unsigned int	color =0;
	unsigned int	count =0;
	
	unsigned int 	x_s = x;
	unsigned int 	x_e ;	
	unsigned int 	y_e ;
	
	FILE 			*pjpg_file;
	
			 int	jpg_fd;
	unsigned int 	jpg_size;
	
	unsigned int 	jpg_width;
	unsigned int 	jpg_height;
	

	if(pjpg_path!=NULL)
	{
		if ((pjpg_file = fopen(pjpg_path, "rb")) == NULL) 
		{
			printf("can't open %s\n", pjpg_path);
			return -1;
		}
	}
	else
	{
		printf("jpg path is null\n");
		return -1;
	}

	/*ע�������*/
	cinfo.err = jpeg_std_error(&jerr);

	/*��������*/
	jpeg_create_decompress(&cinfo);

	/*ָ������������Դ*/		
	jpeg_stdio_src(&cinfo, pjpg_file);
	
	/*���ļ�ͷ��Ϣ*/
	jpeg_read_header(&cinfo, TRUE);

	/*��ʼ����*/
	jpeg_start_decompress(&cinfo);	

	/*���������ݣ��������ж�ȡ*/
	while(cinfo.output_scanline < cinfo.output_height )
	{		
		pcolor_buf = g_color_buf;
		
		/* ÿ�ζ�ȡjpgһ�е�rgbֵ */
		jpeg_read_scanlines(&cinfo,&pcolor_buf,1);
		
		for(i=0; i<cinfo.output_width; i++)
		{
			/* ��ȡrgbֵ */
			color = 		*(pcolor_buf+2);
			color = color | *(pcolor_buf+1)<<8;
			color = color | *(pcolor_buf)<<16;
			
			/* ��ʾ���ص� */
			lcd_draw_point(x,y,color);
			
			pcolor_buf +=3;
			
			x++;
		}
		
		/* ���� */
		y++;			
		
		x = x_s;
	}		
			
	/*�������*/
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);


	/* �ر�jpg�ļ� */
	fclose(pjpg_file);

	return 0;	
}


//LCD��ʾ����ͷת�����rgb����
int lcd_draw_rgb(unsigned int x,unsigned int y,unsigned int rgb_width,unsigned int rgb_height,unsigned char *rgb_buf)   
{
	unsigned int blue, green, red;
	unsigned int color;
	unsigned int x_s = x;
	unsigned int x_e ;	
	unsigned int y_e ;	
	unsigned char i=0;
	unsigned char *prgb_buf = rgb_buf;
	
	x_e = x + rgb_width;
	y_e = y + rgb_height;	
	
	for(;y < y_e; y++)
	{
		for (;x < x_e; x++)
		{
				/* ��ȡ��������ɫ���� */
				color = 		*(prgb_buf+2);
				color = color | *(prgb_buf+1)<<8;
				color = color | *(prgb_buf)<<16;
				
				lcd_draw_point(x, y, color);

				prgb_buf+=3;
		}
		
		x = x_s;
	}			
	
	return 0;
}

//��ʾ����ͷ����
int lcd_draw_camera(unsigned int x,unsigned int y,FrameBuffer framebuf_t)  
{
	int camera_support_format=-1;
	int rt=0;
	unsigned int width,height;
	
	//��ȡ����ͷ֧�ֵĸ�ʽ
	camera_support_format=linux_v4l2_get_format();
	
	if(camera_support_format < 0)
		return -1;
	
	//��ȡ����ͷ��ǰ֧�ֵķֱ���
	rt=linux_v4l2_get_resolution(&width,&height);
	
	if(rt < 0)
		return -1;
	
	//��������ͷ֧�ֵĸ�ʽ����ʾ��LCD��
	if(camera_support_format == V4L2_PIX_FMT_YUYV)		
		lcd_draw_rgb(x,y,width,height,framebuf_t.buf);
	
	if(camera_support_format == V4L2_PIX_FMT_JPEG)		
		lcd_draw_jpg(x,y,framebuf_t.buf,framebuf_t.length);	
	
	return 0;
}



//LCD�ر�
void lcd_close(void)
{
	/* ȡ���ڴ�ӳ�� */
	munmap(g_pfb_memory, FB_SIZE);
	
	/* �ر�LCD�豸 */
	close(g_fb_fd);
}


