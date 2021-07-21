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

int list_add_head(struct list_node * head ,int id,int money)
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
		printf("找到 ID = %d\n",TS->id);
		printf("余额 = %d\n",TS->money);
        printf("照片 = %s\n",TS->picture);
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
        	printf("照片 = %s\n",TS->picture);
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
			return 0;
		}
		
	}
	printf("没有找到对应的节点，删除失败！\n");
	return -1;
}

//修改节点金额
int  money_list_node(struct list_node * head ,int mode ,int id ,int money)
{
	//创建两个替身
	struct list_node *p = NULL;
	struct list_node *q = NULL;
	if(mode == 0)
    {
        for(q=head,p=head->next ; p != NULL ; q = p , p = p->next)
	    {
	    	if(p->id == id)
	    	{
                p->money = money;
	    		printf("找到对应的节点，并且已经修改金额成功！\n");
	    		return 0;
	    	}
    
	    }
	    printf("没有找到对应的节点，修改金额失败！\n");
	    return -1; 
    }
    else if(mode == 1)
    {
        for(q=head,p=head->next ; p != NULL ; q = p , p = p->next)
	    {
		    if(id == p->id)
		    {
                p->id = money;
		    	printf("找到对应的节点，并且已经修改id成功！\n");
		    	return 0;
		    }
    
	    }   
	    printf("没有找到对应的节点，修改id失败！\n");
	    return -1;
    }
	
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
			list_add_head(data0,id,money);
			

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
				printf("输入0删除该条目\n输入1修改该id账上金额\n输入2修改该id\n输入3退出\n");
				scanf("%d",&menu2);
				if(menu2!=3)
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
                        money_list_node(data0,0,id,money);
                    }
                    else if(menu2 == 2)
                    {
                        printf("请输入修改id:\n");
                        scanf("%d",&money);
                        money_list_node(data0,1,id,money);
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


