#include <time.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include "read.h"
#include <string.h>
#include <stdlib.h>

int show_time()
{
    // int hour0=1;
    // int min0=0;
    // int sec0=0;
    // int year0=2015;
    // int month0=1;
    // int day0=1;
    while(1)
    {
    Init_Font();
    Clean_Area(0,0,800,200,0x0000ff00);
    char *wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    time_t timep;
    struct tm *p;
    time(&timep);
    p = gmtime(&timep);
    // printf("%d/%d/%d\n", (1900+p->tm_year), (1+p->tm_mon), p->tm_mday);
    // printf("%s\n%d:%d:%d\n", wday[p->tm_wday], p->tm_hour, p->tm_min, p->tm_sec);

    char str1[10];
    sprintf(str1,"%d年",1900+p->tm_year);
    char str2[10];
    sprintf(str2,"%d月",1+p->tm_mon);
    char str3[10];
    sprintf(str3,"%d日 ",p->tm_mday);
    char str4[10];
    sprintf(str4," %d时",p->tm_hour);
    char str5[10];
    sprintf(str5,"%d分",p->tm_min);
    char str6[10];
    sprintf(str6,"%d秒",p->tm_sec);
    

    char *temp;
    temp = (char *)malloc(sizeof(char)*50);
    strcpy(temp,str1);//(3)
    strcat(temp,str2);
    strcat(temp,str3);
    strcat(temp,wday[p->tm_wday]);
    strcat(temp,str4);
    strcat(temp,str5);
    strcat(temp,str6);

    // int time1,hour1,min1,sec1;
    // time1=(p->tm_hour-hour0)*3600+(p->tm_min-min0)*60+ p->tm_sec-sec0;
    // hour1=time1 / 3600;
    // min1=(time1-hour1*3600) / 60;
    // sec1=time1 - hour1*3600 - min1*60;
    
    // char str7[20];
    // sprintf(str7,"已停车%d小时",hour1);
    // char str8[20];
    // sprintf(str8,"%d分",min1);
    // char str9[20];
    // sprintf(str9,"%d秒",sec1);

    // char *temp2;
    // temp2 = (char *)malloc(sizeof(char)*50);
    // strcpy(temp2,str7);
    // strcat(temp2,str8);
    // strcat(temp2,str9);

    // int pay;//按分钟计时
    // int sec_cnt;
    // if(sec1<30){sec_cnt=0;}
    // else {sec_cnt=1;}
    // pay=hour1*60+min1+sec_cnt;

    // char str10[20];
    // sprintf(str10,"按分钟计费=%d",pay);

    Display_characterX(0,0,temp,0x00ff0000,2);
    // Display_characterX(0,50,temp2,0x00ff0000,2);
    // Display_characterX(0,100,str10,0x00ff0000,2);

    UnInit_Font();
    free(temp);
    // free(temp2);
    sleep(1);
    }
    
    return 0;
}

int show_pay()
{

}