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
    Clean_Area(0,0,800,50,0x00ffffff);
    char *wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    time_t timep;
    struct tm *p;
    time(&timep);
    p = gmtime(&timep);
    // printf("%d/%d/%d\n", (1900+p->tm_year), (1+p->tm_mon), p->tm_mday);
    // printf("%s\n%d:%d:%d\n", wday[p->tm_wday], p->tm_hour, p->tm_min, p->tm_sec);

    char str1[10];
    sprintf(str1,"%dÄê",1900+p->tm_year);
    char str2[10];
    sprintf(str2,"%dÔÂ",1+p->tm_mon);
    char str3[10];
    sprintf(str3,"%dÈÕ",p->tm_mday);
    char str4[10];
    sprintf(str4," %dÊ±",p->tm_hour);
    char str5[10];
    sprintf(str5,"%d·Ö",p->tm_min);
    char str6[10];
    sprintf(str6,"%dÃë",p->tm_sec);
    

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
    // sprintf(str7,"ï¿½ï¿½ï¿½ï¿½Í£ï¿½ï¿½%dÐ¡Ê±",hour1);
    // char str8[20];
    // sprintf(str8,"%dï¿½ï¿½",min1);
    // char str9[20];
    // sprintf(str9,"%dï¿½ï¿½",sec1);

    // char *temp2;
    // temp2 = (char *)malloc(sizeof(char)*50);
    // strcpy(temp2,str7);
    // strcat(temp2,str8);
    // strcat(temp2,str9);

    // int pay;//ï¿½ï¿½ï¿½ã»¨ï¿½ï¿½
    // int sec_cnt;
    // if(sec1<30){sec_cnt=0;}
    // else {sec_cnt=1;}
    // pay=hour1*60+min1+sec_cnt;

    // char str10[20];
    // sprintf(str10,"Í£ï¿½ï¿½ï¿½ï¿½ï¿?=%d",pay);

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

int show_pay(int cast)
{
    Init_Font();
    Clean_Area(0,50,800,50,0x00ffffff);
    char str1[100];
    char str2[50];
    
    do_beep();
    sprintf(str1,"±¾´ÎÍ£³µÒ»¹²Ïû·Ñ%d",cast);
    sprintf(str2,"%d",cast);
    // system("madplay bencitingche.mp3");
    // system("madplay yigongjiaofei.mp3");
    // audioplay(str2);
    // system("madplay yuan.mp3");

    Display_characterX(0,50,str1,0x000000ff,2);
    UnInit_Font();
    return 0;
}