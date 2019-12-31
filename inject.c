#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if(argc != 3) {
        fprintf(stderr, "sigzip-inject: No files provided.\n");
        exit(1);
    }

    int fd, dd, offset;
    char *data, *sigaddr;
    struct stat sbuf, dbuf;

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


    if ((dd = open(argv[2], O_RDONLY)) == -1) {
        perror("open");
        exit(1);
    }

    if (stat(argv[2], &dbuf) == -1) {
        perror("stat");
        exit(1);
    }

    if (dbuf.st_size > UINT16_MAX) {
        fprintf(stderr, "sigzip: injected file too large, maximum size 64k.\n");
		exit(1);
    }

	if ((data = mmap((caddr_t)0, sbuf.st_size, PROT_READ, MAP_SHARED, fd, 0)) == (caddr_t)(-1)) {
		perror("mmap");
		exit(1);
	}

    if ((sigaddr = mmap((caddr_t)0, dbuf.st_size, PROT_READ, MAP_SHARED, dd, 0)) == (caddr_t)(-1)) {
		perror("mmap");
		exit(1);
	}

    if (data[3] & (1<<2)) {
        fprintf(stderr, "sigzip: Cannot add signature - data present.\n");
		exit(1);
    }

    int dfend = 10 + (data[3] & (1<<1));
    char modhead = data[3] | (1<<2);
    
    fwrite(&data[0],sizeof(char),3,stdout);
    fwrite(&modhead,sizeof(char),1,stdout);
    fwrite(&data[4],sizeof(char),dfend-4,stdout);
    fwrite(&dbuf.st_size,sizeof(char),2,stdout);
    fwrite(&sigaddr[0],sizeof(char),dbuf.st_size,stdout);
    fwrite(&data[dfend],sizeof(char),sbuf.st_size-dfend,stdout);
    fflush(stdout);
    munmap(data, sbuf.st_size);
    munmap(sigaddr, dbuf.st_size);
    return 0;
}
