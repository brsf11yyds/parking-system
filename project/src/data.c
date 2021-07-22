#include "read.h"
extern struct list_node * data0;

int list_write(struct list_node * head )
{
	FILE *fp;
 
   	fp = fopen( "data.txt" , "w" );
   	
	struct list_node * TS = NULL;
	for(TS = head->next ; TS != NULL ; TS = TS->next)
	{
		if(TS->id != 0)
		fwrite(TS, sizeof(struct list_node) , 1, fp );
	}
   	fclose(fp);
}

int list_read(struct list_node * head )
{
	FILE *fp;
 
   	fp = fopen( "data.txt" , "r+" );
   	if(feof(fp) != 1)
	{
		struct list_node * TSN;
		struct list_node * TSO;
		TSO = head;
		while(!feof(fp))
		{
			TSN = (struct list_node*)malloc(sizeof(struct list_node));
			fread(TSN, sizeof(struct list_node) , 1, fp );
			TSO->next = TSN;
			TSO = TSN;
		}
		TSN->next = NULL;
		return 0;
		
	}
	else return -1;
   	fclose(fp);
}

int list_add_head(struct list_node * head ,int id,unsigned money,int cat)
{
	struct list_node * p = NULL;
	p = (struct list_node*)malloc(sizeof(struct list_node));
	if(p == NULL)
	{
		printf("申请空间失败！\n");
		return -1;
	}
	
	p->id = id;
	p->next = head->next;
	p->money = money;
	p->cat = cat;
	p->state = 0;
	head->next = p;
	
	return 0;	
}

//显示每一个节点的数据
int show_list_node(struct list_node * head)
{
	if(head->next == NULL)
	{
		printf("没有节点!\n");
		return -1;
	}
	
	//创建一个替身
	struct list_node * TS = NULL;
	for(TS = head->next ; TS != NULL ; TS = TS->next)
	{
		printf("找到 ID  = %d\n",TS->id);
		printf("余额     = %d\n",TS->money);
        printf("照片in   = %s\n",TS->in_picture);
		printf("照片out  = %s\n",TS->out_picture);
		printf("车的状态 = %d\n",TS->state);
		printf("卡的类型 = %d\n",TS->cat);
	}
	
	return 0;
}

//查找节点里面有没有这个数据
int find_list_node(struct list_node * head , int id)
{
	int ok;
	if(head->next == NULL)
	{
		printf("没有节点!\n");
		return -1;
	}
	
	//创建一个替身
	struct list_node * TS = NULL;
	for(TS = head->next ; TS != NULL ; TS = TS->next)
	{
		if(TS->id == id)
		{
			printf("找到 ID = %d\n",TS->id);
			printf("余额 = %d\n",TS->money);
        	printf("照片in   = %s\n",TS->in_picture);
			printf("照片out  = %s\n",TS->out_picture);
			printf("车的状态 = %d\n",TS->state);
			printf("卡的类型 = %d\n",TS->cat);
			ok=1;
			break;
		}
		
	}
	if(ok==0) 
	{
		printf("没有！\n");
		return 0;
	}
	else return 1;
}

//删除节点//通过遍历每一个节点，通过节点的data数据和num进行对比，来确定删除那个节点
int  delete_list_node(struct list_node * head , int id)
{
	//创建两个替身
	struct list_node *p = NULL;
	struct list_node *q = NULL;
	
	for(q=head,p=head->next ; p != NULL ; q = p , p = p->next)
	{
		if(p->id == id)
		{
			q->next =  p->next;
			free(p);
			printf("找到对应的节点，并且已经删除！\n");
			list_write(data0);
			return 0;
		}
		
	}
	printf("没有找到对应的节点，删除失败！\n");
	
	return -1;
}

//修改节点金额
int  money_list_node(struct list_node * head ,int id ,unsigned money)
{
	//创建两个替身
	struct list_node *p = NULL;
	struct list_node *q = NULL;
    for(q=head,p=head->next ; p != NULL ; q = p , p = p->next)
	{
		if(p->id == id)
		{
            p->money = money;
			printf("找到对应的节点，并且已经修改金额成功！\n");
			list_write(data0);
			return 0;
		}

	}
	printf("没有找到对应的节点，修改金额失败！\n");
	
	return -1;	
}
//修改节点id
int  id_list_node(struct list_node * head ,int id ,int new_id)
{
	//创建两个替身
	struct list_node *p = NULL;
	struct list_node *q = NULL;
    for(q=head,p=head->next ; p != NULL ; q = p , p = p->next)
	{
	    if(id == p->id)
	    {
            p->id = new_id;
	    	printf("找到对应的节点，并且已经修改id成功！\n");
			list_write(data0);
	    	return 0;
	    }

	}   
	printf("没有找到对应的节点，修改id失败！\n");
	return -1;	
}
//修改节点状态
int  state_list_node(struct list_node * head ,int id ,int state)
{
	//创建两个替身
	struct list_node *p = NULL;
	struct list_node *q = NULL;
    for(q=head,p=head->next ; p != NULL ; q = p , p = p->next)
	{
	    if(id == p->id)
	    {
            p->state = state;
	    	printf("找到对应的节点，并且已经修改state成功！\n");
			list_write(data0);
	    	return 0;
	    }

	}   
	printf("没有找到对应的节点，修改state失败！\n");
	return -1;	
}
//修改节点卡种类
int  cat_list_node(struct list_node * head ,int id ,int cat)
{
	//创建两个替身
	struct list_node *p = NULL;
	struct list_node *q = NULL;
    for(q=head,p=head->next ; p != NULL ; q = p , p = p->next)
	{
	    if(id == p->id)
	    {
            p->cat = cat;
	    	printf("找到对应的节点，并且已经修改id成功！\n");
			list_write(data0);
	    	return 0;
	    }

	}   
	printf("没有找到对应的节点，修改id失败！\n");
	return -1;	
}
//修改节点时间
int  time_list_node(struct list_node * head ,int id ,time_t time)
{
	//创建两个替身
	struct list_node *p = NULL;
	struct list_node *q = NULL;
    for(q=head,p=head->next ; p != NULL ; q = p , p = p->next)
	{
	    if(id == p->id)
	    {
            p->intial_time = time;
	    	printf("找到对应的节点，并且已经修改time成功！\n");
			list_write(data0);
	    	return 0;
	    }

	}   
	printf("没有找到对应的节点，修改time失败！\n");
	return -1;	
}
//修改节点图片
int  pic_list_node(struct list_node * head ,int id ,char *picture,int inout)
{
	//创建两个替身
	struct list_node *p = NULL;
	struct list_node *q = NULL;
    for(q=head,p=head->next ; p != NULL ; q = p , p = p->next)
	{
	    if(id == p->id)
	    {
			if(inout == CAR_IN)
			{
				strcpy(p->in_picture,picture);
			}
			else
			{
				strcpy(p->out_picture,picture);
			}
            
	    	printf("找到对应的节点，并且已经修改time成功！\n");
			list_write(data0);
	    	return 0;
	    }

	}   
	printf("没有找到对应的节点，修改time失败！\n");
	return -1;	
}

//查询节点状态
int  state_list_node_2(struct list_node * head ,int id ,int *state)
{
	//创建两个替身
	struct list_node *p = NULL;
	struct list_node *q = NULL;
    for(q=head,p=head->next ; p != NULL ; q = p , p = p->next)
	{
	    if(id == p->id)
	    {
            *state = p->state;
	    	printf("找到对应的节点，并且已经查询state成功！\n");
	    	return 0;
	    }

	}   
	printf("没有找到对应的节点，查询state失败！\n");
	return -1;	
}

//查询节点余额
int  money_list_node_2(struct list_node * head ,int id ,int *money)
{
	//创建两个替身
	struct list_node *p = NULL;
	struct list_node *q = NULL;
    for(q=head,p=head->next ; p != NULL ; q = p , p = p->next)
	{
	    if(id == p->id)
	    {
            *money = p->money;
	    	printf("找到对应的节点，并且已经查询money成功！\n");
	    	return 0;
	    }

	}   
	printf("没有找到对应的节点，查询money失败！\n");
	return -1;	
}

//查询节点时间
int  time_list_node_2(struct list_node * head ,int id ,time_t *time)
{
	//创建两个替身
	struct list_node *p = NULL;
	struct list_node *q = NULL;
    for(q=head,p=head->next ; p != NULL ; q = p , p = p->next)
	{
	    if(id == p->id)
	    {
            *time = p->intial_time ;
	    	printf("找到对应的节点，并且已经查询time成功！\n");
	    	return 0;
	    }

	}   
	printf("没有找到对应的节点，查询time失败！\n");
	return -1;	
}

//查询节点类型
int  cat_list_node_2(struct list_node * head ,int id ,int *cat)
{
	//创建两个替身
	struct list_node *p = NULL;
	struct list_node *q = NULL;
    for(q=head,p=head->next ; p != NULL ; q = p , p = p->next)
	{
	    if(id == p->id)
	    {
            *cat = p->cat ;
	    	printf("找到对应的节点，并且已经查询cat成功！\n");
	    	return 0;
	    }

	}   
	printf("没有找到对应的节点，查询time失败！\n");
	return -1;	
}

int data()
{
    data0 = (struct list_node*)malloc(sizeof(struct list_node));
	data0->id = 0;   
	data0->money = 0;
	data0->next = NULL;	
	list_read(data0);
	
	int find;
	int menu;
	int menu2;
	int money;
	int id;
	int cat;

	printf("欢迎使用车库管理系统\n输入0显示所有条目\n输入1进行增加条目\n输入2进行检索并操作条目\n输入3退出\n");
	scanf("%d",&menu);

	while(menu != 3)
	{
		if(menu == 0)
		show_list_node(data0); 
		else if(menu == 1)
		{
			printf("请新卡的ID:\n");
			scanf("%d",&id);
			
			
			printf("请输入充值的金额:\n");
			scanf("%d",&money);
			list_add_head(data0,id,money,1);
			printf("请把ID写入卡内:\n");
			write_card(id);

			list_write(data0);
			printf("欢迎使用车库管理系统\n输入0显示所有条目\n输入1进行增加条目\n输入2进行检索并操作条目\n输入3退出\n");
		}
		else if(menu == 2)
		{
			printf("请输入卡的ID:\n");
			scanf("%d",&id);
			find = find_list_node(data0,id);
			if(find)
			{
				printf("输入0删除该条目\n输入1修改该id账上金额\n输入2修改该id\n输入3改卡类型\n输入4退出\n");
				scanf("%d",&menu2);
				if(menu2!=4)
				{
					if(menu2 == 0)
					{
						printf("%d",id);
						delete_list_node(data0,id);
					}
                    else if(menu2 == 1)
                    {
                        printf("请输入修改金额:\n");
                        scanf("%d",&money);
                        money_list_node(data0,id,money);
                    }
                    else if(menu2 == 2)
                    {
                        printf("请输入修改id:\n");
                        scanf("%d",&money);
                        id_list_node(data0,id,money);
                    }
					else if(menu2 == 3)
                    {
                        printf("请输入修改类型:\n");
                        scanf("%d",&money);
                        cat_list_node(data0,id,money);
                    }
				}
			}
			list_write(data0);
		}
		printf("输入0显示所有条目\n输入1进行增加条目\n输入2进行检索条目\n输入3退出\n");
		scanf("%d",&menu);
	}
	return 0;
}



