#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#define SB_OFFSET	1024

#define SB_SIZE		1020

#define M_OFFSET	0x38

int main(int argc, char **argv)
{
	char corrupt[2] = {0x13, 0x37};

	char *dev = NULL;
	int c;

	while((c = getopt(argc, argv, "d:")) != -1)
	{
		switch(c) {

		case 'd':
			dev = optarg;
			break;
		case '?':
			if(optopt == 'c')
				fprintf(stderr, "-c requires an argument ..\n");
			break;
		}
	}

	if(dev == NULL) {
		fprintf(stderr, "You must specify a device to open (sb -d /dev/device) ..\n");
		return -1;
	}

	char sb_bytes[SB_SIZE];

	int fd = open(dev, O_RDWR);

	if(fd < 0) {
		fprintf(stderr, "Error opening device %s\n", dev);
		return -1;
	}

	lseek(fd, SB_OFFSET, SEEK_SET);

	read(fd, &sb_bytes, (SB_SIZE));
	
	int x;
	if(sb_bytes[M_OFFSET] == 0x53 && sb_bytes[M_OFFSET+1] == 0xEF) {
		fprintf(stdout, "Found magic number at offset: %u-%u\n", M_OFFSET, (M_OFFSET+1));
	} else {
		fprintf(stderr, "%s is corrupt (bad magic number)\n", dev);
		goto clean;
	}

	fprintf(stdout, "Do you want to corrupt the superblock of %s? (Y/n)\n", dev);

	char option[8];

	scanf("%s", option);

	if(strstr(option, "Y") != NULL || strstr(option, "y") != NULL) {

		lseek(fd, (SB_OFFSET + M_OFFSET), SEEK_SET);
			
		int ret = write(fd, &corrupt, 2);
		
		if(ret < 0) {
			fprintf(stderr, "Failed to write to %s: %s\n", dev, strerror(errno));
			goto clean;
		}
	} 

clean:
	close(fd);
	fsync(fd);
	
	return 0;
}
