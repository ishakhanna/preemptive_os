#define MYEPERM        1  /* Operation not permitted */
#define MYENOENT       2  /* No such file or directory */
#define MYESRCH        3  /* No such process */
#define MYEINTR        4  /* Interrupted system call */
#define MYEBADF        9  /* Bad file number */
#define MYECHILD      10  /* No child processes */
#define MYENOMEM      12  /* Out of memory */
#define MYEACCES      13  /* Permission denied */
#define MYEFAULT      14  /* Bad address */
#define MYEEXIST      17  /* File exists */
#define MYENOTDIR     20  /* Not a directory */
#define MYEISDIR      21  /* Is a directory */
#define MYEINVAL      22  /* Invalid argument */
#define MYEFBIG       27  /* File too large */
#define MYESPIPE      29  /* Illegal seek */
#define MYENOCMD      34  /* Command Not Found */
#define MYEMFILE      24      /* Too many open files */
char *ERROR_MSG[] = {"Operation not permitted", "No such file or directory", "No such process", "Interrupted system call", "Some Error Occured", "Some Error Occured", "Some Error Occured", "Some Error Occured", "Bad file number", "No child process", "Some Error Occured", "Out of memory", "Permisson denied", "Bad address", "Some Error Occured", "Some Error Occured", "File exists", "Some Error Occured", "Some Error Occured", "Not a directory", "Is a directory", "Invalid argument", "Some Error Occured", "Too many open files", "Some Error Occured", "Some Error Occured", "File too large", "Some Error Occured", "Illegal seek", "Some Error Occured", "Some Error Occured", "Some Error Occured", "Some Error Occured", "Some Error Occured", "Command Not Found"};
