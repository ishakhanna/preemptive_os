#define MYEPERM        1  /* Operation not permitted */
#define MYENOENT       2  /* No such file or directory */
#define MYESRCH        3  /* No such process */
#define MYEINTR        4  /* Interrupted system call */
#define MYEIO          5  /* I/O error */
#define MYENXIO        6  /* No such device or address */
#define MYE2BIG        7  /* Argument list too long */
#define MYENOEXEC      8  /* Exec format error */
#define MYEBADF        9  /* Bad file number */
#define MYECHILD      10  /* No child processes */
#define MYEAGAIN      11  /* Try again */
#define MYENOMEM      12  /* Out of memory */
#define MYEACCES      13  /* Permission denied */
#define MYEFAULT      14  /* Bad address */
#define MYENOTBLK     15  /* Block device required */
#define MYEBUSY       16  /* Device or resource busy */
#define MYEEXIST      17  /* File exists */
#define MYEXDEV       18  /* Cross-device link */
#define MYENODEV      19  /* No such device */
#define MYENOTDIR     20  /* Not a directory */
#define MYEISDIR      21  /* Is a directory */
#define MYEINVAL      22  /* Invalid argument */
#define MYENFILE      23  /* File table overflow */
#define MYEMFILE      24  /* Too many open files */
#define MYENOTTY      25  /* Not a typewriter */
#define MYETXTBSY     26  /* Text file busy */
#define MYEFBIG       27  /* File too large */
#define MYENOSPC      28  /* No space left on device */
#define MYESPIPE      29  /* Illegal seek */
#define MYEROFS       30  /* Read-only file system */
#define MYEMLINK      31  /* Too many links */
#define MYEPIPE       32  /* Broken pipe */
#define MYEDOM        33  /* Math argument out of domain of func */
#define MYERANGE      34  /* Math result not representable */
#define MYENOCMD      34  /* Command Not Found */

char *ERROR_MSG[] = {"Operation not permitted", "No such file or directory", "No such process", "Interrupted system call", "I/O error", "No such device or address", "Argument list too long", "Exec format error", "Bad file number", "No child process", "Try again", "Out of memory", "Permisson denied", "Bad address", "Block device required", "Device or resource busy", "File exists", "Cross-device link", "No such device", "Not a directory", "Is a directory", "Invalid argument", "File table overflow", "Too many open files", "Not a typewriter", "Text file busy", "File too large", "No space left on device", "Illegal seek", "Read-only file system", "Too many links", "Broken pipe", "Math argument out of domain of func", "Math result not representable", "Command Not Found"};
