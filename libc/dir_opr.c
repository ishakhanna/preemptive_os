#include "../include/sys/syscall.h"
#include "../include/syscall.h"
#include "../include/stdlib.h"
#include <stdio.h>

void *memset(void *s, int c, size_t n);

int chdir(const char *path){
	int err = (int) syscall_1(SYS_chdir, (uint64_t)path);
	
	if(err < 0){
		errno = (-1)*err;
		return -1;
	}
	else
		return 0;
}

char *getcwd(char *buf, size_t size){
	int ret = (int) syscall_2(SYS_getcwd, (uint64_t)buf, (uint64_t) size);

	if(ret < 0){
		errno = (-1)*ret;
		return NULL;
	}
	else
		return buf;
}
void *opendir(const char *name){
	int fd = (int) open(name, O_DIRECTORY);
	//size_t alloc1 = 1024;
//	size_t alloc1 = sizeof(struct dirent);
	DIR *dirp = malloc(sizeof(struct Dir));
	dirp->fd = fd;
	dirp->offset = 0;
//	dirp->allocation = alloc1;
	dirp->bytes_read = 0;
	return dirp;
}

struct dirent *readdir(void *dp)
{
    
    DIR *ds = (DIR *)dp;
    int nread = 0;
    int BUF_SIZE = 2048;
	struct dirent *d = malloc(sizeof(struct dirent));    
    
    if(ds->offset == 0 || ds->offset >= ds->bytes_read){
		ds->data = malloc(BUF_SIZE);
		memset(ds->data, 0, BUF_SIZE);
        	nread = syscall_3(SYS_getdents, ds->fd, (uint64_t)(ds->data), BUF_SIZE);
		if(nread < 0){
			errno = (-1)*nread;
			printf("ERROR\n");
			return NULL;
		}else if(nread == 0)
			return NULL;
		ds->bytes_read = nread;
		ds->offset = 0;
	}

         d = (struct dirent *)(ds->data + (ds->offset));
	ds->offset += d->d_reclen;
    return d;
}

int closedir(void *ds){
	DIR *new_ds = (DIR *)ds;
	if(new_ds == NULL){
		printf("\nNULL\n");
		errno = 9;
		return -1;
	}
	int close_res = (int) syscall_1(SYS_close, (uint64_t) new_ds->fd);
	free(new_ds);
	if(close_res < 0)
		return -1;
	return 0;
}
