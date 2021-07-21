#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *lbw(void *arg)
{   printf("%s\n",(char *)arg);
    while(1)
    {
    printf("nmsl\n");
    sleep(1);  
    printf("2\n");   
    pthread_exit(NULL);  
    }   
    int num;

}

int main()
{
    pthread_t thread;
    pthread_create(&thread , NULL, lbw,"help" );

    char **p;
    pthread_join(thread,NULL);

    while(1)
    {
        printf("jz?\n");
        sleep(1);
    }
    return 0;
}