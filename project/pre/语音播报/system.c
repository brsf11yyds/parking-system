#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int music(unsigned char *p)
{
int n;
    for(n=0;p[n];n++)
        { 
            switch(p[n])
            {
            case '1':system("madplay 1.mp3");break;
            case '2':system("madplay 2.mp3");break;
            case '3':system("madplay 3.mp3");break;
            case '4':system("madplay 4.mp3");break;
            case '5':system("madplay 5.mp3");break;
            case '6':system("madplay 6.mp3");break;
            case '7':system("madplay 7.mp3");break;
            case '8':system("madplay 8.mp3");break;
            case '9':system("madplay 9.mp3");break;
            case '0':system("madplay 0.mp3");break;
            }
        }
    
    return 0;
}