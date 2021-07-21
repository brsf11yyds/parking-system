#include "read.h"
#include<pthread.h>

void *time_show(void *arg)
{  
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

	printf("Start\n");

	sleep(3);

	time_t timep;
	time(&timep);

	time_list_node(data0,114514,timep);

	write_card(114514);

	while(1)
	{
		clearlcd();
    	show_bmp("welcome.bmp"); //欢迎界面
		system("madplay welcome.mp3");

    	int x,y;
		int select=0;
		int id;
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
		if(select == 1)
		{
			read_card(&id);
			int find = find_list_node(data0,id);
			if(find)
			{
				int state;
				state_list_node_2(data0,id ,&state);
				//出门，结算
				if(state == 0)
				{
					time_t timep,init_time;
					state_list_node(data0,id,1);
					time_list_node_2(data0,id,&init_time);
    				struct tm *p;
					struct tm *init;
    				time(&timep);

    				p = gmtime(&timep);
					init = gmtime(&init_time);

					int cost;
					cost = timep - init_time;

					int temp_money;
					
					money_list_node_2(data0,id,&temp_money);
					if(temp_money>=cost)
					{
						money_list_node(data0,id,temp_money - cost);
					}
					else
					{
						printf("穷鬼给爷爬\n");
					}
				}
				//进门
				else
				{
					state_list_node(data0,id,0);
				}
			}
			else
			printf("未找到该账户");
		}
		else
		{

		}
		//card(select);//信息监测与进车
	}

    return 0;
}