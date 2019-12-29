#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if(argc != 2) {
        fprintf(stderr, "sigzip: No file provided.\n");
        exit(1);
    }

    int fd, offset;
    char *data;
    struct stat sbuf;

    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        perror("open");
        exit(1);
    }

    if (stat(argv[1], &sbuf) == -1) {
        perror("stat");
        exit(1);
    }

    if (sbuf.st_size-1 < 3) {
		fprintf(stderr, "sigzip: Malformed gzip.\n");
		exit(1);
	}
	
	if ((data = mmap((caddr_t)0, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == (caddr_t)(-1)) {
		perror("mmap");
		exit(1);
	}

    int dfoffset = 10 + (data[3] & (1<<1));
    if (!(data[3] & (1<<2))) {
        fprintf(stderr, "sigzip: No extra data present.\n");
		exit(1);
    }

    unsigned short hleng = (short)(((short)data[dfoffset+1]) << 8) | data[dfoffset];
    fwrite(&data[dfoffset+2],sizeof(char),hleng,stdout);
    fflush(stdout);
    munmap(data, sbuf.st_size);
    return 0;
}
