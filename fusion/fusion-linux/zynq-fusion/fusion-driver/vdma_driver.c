/**
 * drive the VDMA in Linux user mode
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/times.h>

#define VDMA_BASSADDR 0x43000000
#define OUT_BASEADDR 0x1f800000
#define HEAD_SIZE 0x36
#define DATA_SIZE 0xe1000

unsigned char *map_base;
unsigned char *out_base;
unsigned char *in_base;
int vdma_idle()
{
	int *temp = (unsigned int *)(map_base + 0x34);
	printf("%x\n", *temp);
	return (*temp) & 0x00001000;
}
int main()
{
	FILE *f;
	int n, fd;

	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd == -1)
	{
		return (-1);
	}

	map_base = mmap(NULL, 0xff, PROT_READ | PROT_WRITE, MAP_SHARED, fd, VDMA_BASSADDR);
	out_base = mmap(NULL, 0x200000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, OUT_BASEADDR);
	in_base = mmap(NULL, 0x200000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, OUT_BASEADDR + 0x200000);

	if (map_base == 0)
	{
		printf("NULL pointer!\n");
	}
	else
	{
		printf("Successfull!\n");
	}

	FILE *bmp;

	char head_buff[HEAD_SIZE + 1] = {0};
	char data_buff[DATA_SIZE + 1] = {0};
	int num_read = 0;
	if ((bmp = fopen("./test.bmp", "r")) != NULL)
	{
		num_read = fread(head_buff, 1, HEAD_SIZE, bmp);
		printf("%d byte read \n", num_read);
		num_read = fread(in_base, 1, DATA_SIZE, bmp);
		printf("%d byte read \n", num_read);
		printf("data_buff --%lx \n", data_buff);
	}
	else
	{
		printf("open the bmp file error \n");
	}

	int frame_count = 2;
	int *temp32 = 0;
	temp32 = (unsigned int *)(map_base + 0x0);
	*temp32 = 0x4;
	*temp32 = 0x8;
	temp32 = (unsigned int *)(map_base + 0x5c);
	*temp32 = 0x1f800000 + 0x200000;
	temp32 = (unsigned int *)(map_base + 0x60);
	*temp32 = 0x1f800000 + 0x600000;

	temp32 = (unsigned int *)(map_base + 0x54);
	*temp32 = 640 * 3;
	temp32 = (unsigned int *)(map_base + 0x58);
	*temp32 = 0x01000780;
	temp32 = (unsigned int *)(map_base + 0x0);
	*temp32 = 0x03;

	temp32 = (unsigned int *)(map_base + 0xac);
	*temp32 = 0x1f800000;
	temp32 = (unsigned int *)(map_base + 0xb0);
	*temp32 = 0x1f800000 + 0x400000;

	temp32 = (unsigned int *)(map_base + 0xa4);
	*temp32 = 640 * 3;
	temp32 = (unsigned int *)(map_base + 0xa8);
	*temp32 = 0x01000780;
	temp32 = (unsigned int *)(map_base + 0x30);
	*temp32 = 0x3;

	temp32 = (unsigned int *)(map_base + 0x50);
	*temp32 = 480;
	temp32 = (unsigned int *)(map_base + 0xa0);
	*temp32 = 480;
	vdma_idle();

	int count = 0;

	struct timeval tpstart, tpend;
	double timeuse;
	gettimeofday(&tpstart, NULL);

	while (0 == vdma_idle())
	{
		count++;
	}

	gettimeofday(&tpend, NULL);
	timeuse = 1000000 * (tpend.tv_sec - tpstart.tv_sec) + tpend.tv_usec - tpstart.tv_usec;
	printf("use time :%f  us\n", timeuse);

	printf("count----%d\n", count);
	FILE *out_bmp = NULL;
	if ((out_bmp = fopen("./out.bmp", "w+")) != NULL)
	{
		int num_write = 0;
		num_write = fwrite(head_buff, 1, HEAD_SIZE, out_bmp);
		printf("%x byte write \n", num_write);
		num_write = fwrite(out_base, 1, DATA_SIZE, out_bmp);
		printf("%x byte write \n", num_write);
	}
	else
	{
		printf("open the out bmp fail \n");
	}

	temp32 = (unsigned int *)(map_base + 0x0);
	*temp32 = 0x4;
	temp32 = (unsigned int *)(map_base + 0x30);
	*temp32 = 0x4;

	close(fd);
	fclose(bmp);
	fclose(out_bmp);
	munmap(map_base, 0xff);
	munmap(in_base, 0x200000);
	munmap(out_base, 0x200000);
	return (1);
}
