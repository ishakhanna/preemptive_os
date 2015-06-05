#include "../include/sys/syscall.h"
#include "../include/syscall.h"
#include "../include/stdlib.h"

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
	void *ret;
	int fd = (int) open(name, O_DIRECTORY);
	//size_t alloc1 = 1024;
	size_t alloc1 = sizeof(struct dirent);
	DIR *dirp = malloc(sizeof(struct Dir) + alloc1);
	dirp->fd = fd;
	dirp->allocation = alloc1;
	return dirp;
}

struct dirent *readdir(void *dp)
{
    
    DIR *ds = (DIR *)dp;
    int nread = 0;
    int BUF_SIZE = 1024;
	struct dirent *d = malloc(sizeof(struct dirent));    
    char buf[BUF_SIZE];

    for(int i = 0;i<BUF_SIZE;i++)
        buf[i]= '0';
    
    int i;
    
        nread = syscall_3(SYS_getdents, ds->fd, (uint64_t)(buf), BUF_SIZE);
    if (nread == -1)
        printf("ERROR!!!!\n");
    else{
    	buf[nread] = '\0';
            printf("--------------- nread=%d ---------------\n", nread);
     printf("inode#   d_reclen  d_off   d_name\n");
     i = 0;
     while (i < nread) {
         d = (struct dirent *) (buf + i);
         printf("%d  ", d->d_ino);
         printf("%d %d  %s\n", d->d_reclen,
                (int) d->d_off, d->d_name);
         i += d->d_reclen;
     }
    }
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
