#ifndef _DEFS_H
#define _DEFS_H

#define NULL ((void*)0)

typedef unsigned long  uint64_t;
typedef          long   int64_t;
typedef unsigned int   uint32_t;
typedef          int    int32_t;
typedef unsigned short uint16_t;
typedef          short  int16_t;

typedef struct Dir{
        int fd;
     //   size_t allocation;
        uint64_t offset;
        char *data;
      //  size_t size;
        uint64_t bytes_read;
        struct dirent *d;
       // int finished;
}DIR;

#endif
