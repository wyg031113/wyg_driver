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
	if(argc !=2 ){
		printf("Usage:app /dev/memdevX");
		return -1;
	}
	int fd = open(argv[1], O_RDWR);
	if(fd < 0){
		printf("open memdev0 failed:%s.\n", strerror(errno));
		return -1;
	}
	printf("open memdev0 successfully!\n");

	char buf[1024];
	int ret = 0;

	ret = read(fd, buf, 1024);
	printf("read %d bytes.\n", ret);

	ret = write(fd, buf, 1024);
	printf("write %d bytes.\n", ret);

	close(fd);
	return 0;
}
