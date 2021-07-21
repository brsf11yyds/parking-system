#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
int main()
{
    struct timeval tv,tv1;
    struct timezone tz,tz1;
    gettimeofday(&tv,&tz);
    printf("tv.tv_sec=%ld\n",tv.tv_sec);
    sleep(3);
    gettimeofday(&tv1,&tz1);
    printf("tv1.tv_sec=%ld\n",tv1.tv_sec);

    printf("___________________\n");
    printf("tv1.tv_sec-tv.tv_sec = %ld\n",tv1.tv_sec-tv.tv_sec);

}