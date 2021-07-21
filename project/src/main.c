#include "read.h"

void *time_show(void *arg)
{   printf("%s\n",(char *)arg);
    while(1)
    {
    	show_time();
    	sleep(1);  
    }   
}
void *manage_show(void *arg)
{   
	data();
}


int main()
{   
	pthread_t thread;
    pthread_create(&thread , NULL, time_show, NULL );
	pthread_t thread2;
	pthread_create(&thread2 , NULL, manage_show, NULL );
	while(1)
	{
		clearlcd();
    	show_bmp("welcome.bmp"); //欢迎界面
		system("madplay welcome.mp3");

    	int x,y;
		int select=0;
		while(1)  //临时与普通卡选择
		{		
			touch(&x ,&y); 
			if(30<x && x<280 && 130<y && y<250)
			{
				show_bmp("putongka.bmp");select=1;break;
			}
			if(620<x && x<870 && 130<y && y<250)
			{
				show_bmp("linshika.bmp");select=2;break;
			}		
		}

		card(select);//信息监测与进车
	}

    return 0;
}