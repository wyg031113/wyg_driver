/*************************************************************************
    > File Name: app.c
    > Author: wyg
    > Mail: wyg_0802@126.com 
    > Created Time: 2016年12月10日 星期六 10时13分02秒
 ************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	int fd = open("/dev/memdev0", O_RDWR);
	if(fd < 0){
		printf("open memdev0 failed:%s.\n", strerror(errno));
		return -1;
	}
	sleep(20);
	printf("open memdev0 successfully!\n");
	close(fd);
	return 0;
}
