int open(const char* pathname, int flags)
{

/*if(pathname == NULL)
{
        return -22; //EINVAL
}*/

int result;
 __asm__ __volatile__(
     "syscall"
     : "=a" (result)
     : "a"(2), "d"(pathname), "c"(flags)
);

return result;
}

