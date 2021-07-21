#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>

int do_beep()
{
	int b_fd;
	b_fd = open("/dev/beep",O_RDWR);
	if(b_fd < 0)
	{
		printf("open b_fd error!\n");
	}
	
	int i;
	for(i=0;i<5;i++)
	{
		ioctl(b_fd,0,1); //on
		usleep(500000);//0.5S
		ioctl(b_fd,1,1); //off
		usleep(500000);//0.5S
	}
	
	close(b_fd);
	
	return 0;
}

int main()
{
	do_beep();
	return 0;
}