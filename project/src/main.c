#include "read.h"
#include<pthread.h>

static int temp_base = 114514;
static int temp_bias = 1;

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

void *system_wp(void *inst)
{
	printf("inst = %s\n",(char*)inst);
	system((const char*)inst);
}

void play_music(char* path)
{
	char inst[50] = "madplay ";
	
	strcat(inst,path);
	pthread_t thread;
    switch(pthread_create(&thread , NULL, system_wp, inst))
	{
		case(EAGAIN): printf("Too much thread!\n");break;
		case(EINVAL): printf("EINVAL\n");break;
		case(EPERM):  printf("EPERM\n");break;
		case(0):			  printf("Success!\n");
		default:      printf("Default!\n");
	}
	
}




int main()
{   
	

	clearlcd();
	show_bmp("welcome.bmp"); //欢迎界面
	sleep(1);
	do_beep();
	
	// pthread_t thread;
    // pthread_create(&thread , NULL, time_show, NULL );
	pthread_t thread2;
	pthread_create(&thread2 , NULL, manage_show, NULL );

	//play_music("welcome.mp3");
	printf("Start\n");

	sleep(3);

	time_t timep;
	time(&timep);


	while(1)
	{
		show_bmp("welcome.bmp");
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
				int cat;
				cat_list_node_2(data0,id,&cat);
				if(cat != select)
				{
					printf("狗罕见滚回停车场\n");
					continue;
				}
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

					printf("Cost = %d\n",cost);
					show_pay(cost);

					syasinn(id,CAR_OUT);
					sleep(5);

					int temp_money;
					
					money_list_node_2(data0,id,&temp_money);
					if(temp_money>=cost)
					{
						money_list_node(data0,id,temp_money - cost);
						continue;
					}
					else
					{
						printf("穷鬼给爷爬\n");
					}
				}
				//进门
				else
				{
					time_t timep;
    				time(&timep);
					state_list_node(data0,id,0);
					time_list_node(data0,id,timep);
					syasinn(id,CAR_IN);
					sleep(5);
					continue;
				}
			}
			else
			printf("未找到该账户");
		}
		else
		{
			read_card(&id);
			if(id == 0)
			{
				write_card(temp_base+temp_bias);
				list_add_head(data0,temp_base+temp_bias,0,select);
				time_t timep;
    			time(&timep);
				time_list_node(data0,temp_base+temp_bias,timep);
				syasinn(temp_base+temp_bias,CAR_IN);
				sleep(5);
				temp_bias++;
			}
			else
			{
				int find = find_list_node(data0,id);
				if(find)
				{
					int cat;
					cat_list_node_2(data0,id,&cat);
					if(cat != select)
					{
						printf("狗罕见滚回停车场\n");
						continue;
					}

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
					delete_list_node(data0, id);
					syasinn(id,CAR_OUT);
					sleep(5);
					show_pay(cost);
					printf("Cost = %d\n",cost);
					write_card(0);
					
					
				}

			}
			
		}
		//card(select);//信息监测与进车
	}

    return 0;
}