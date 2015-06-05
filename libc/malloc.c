#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/syscall.h"
#define TRUE 1
#define FALSE 0
//#define NULL 0
typedef struct block_info
{
	int available;
	size_t size;
	struct block_info *next;
} block;
void *head= NULL; 
//block *first_fit_block(int size);
block *first_fit_block(size_t size)
{
	
        block *curr_ptr=head;
        while(curr_ptr!=NULL)
        {
                if(curr_ptr->size >=  size && curr_ptr->available == TRUE)
                        return curr_ptr;
                curr_ptr=curr_ptr->next;
        }
        return NULL;
}


void *malloc(size_t size)
{
	block *bmeta;
	uint64_t begin = 0;
	block *end_segment;
	uint64_t size_to_allocate=0;
	if(size<= 0)
		return NULL;
	if(head!= NULL)
	{
		bmeta=first_fit_block(size);
		if(bmeta!= NULL)
		{
			bmeta->available=FALSE;
			//bmeta=bmeta+1;
		}
		else
		{
			// ask OS for space
			bmeta = (block *)syscall_1(SYS_brk, begin);
	/*		__asm__ __volatile__(
        	 	"syscall"
        	 	: "=a" (bmeta)
	       		: "a"(SYS_brk), "D"(begin)
			);*/

			size_to_allocate=(uint64_t)bmeta;
			size_to_allocate+=(uint64_t)size+(uint64_t)sizeof(block);
			 
			end_segment = (block *)syscall_1(SYS_brk, size_to_allocate);
			/*__asm__ __volatile__(
                 	"syscall"
                 	: "=a" (end_segment)
                 	: "a"(SYS_brk), "D"(size_to_allocate)
                	);*/

		if((uint64_t)end_segment==(uint64_t)bmeta)
		{	
			return NULL;
		}


			bmeta->available=FALSE;
			block *temp=head;
			head=bmeta;
			bmeta->next=temp;
			bmeta->size=size;
			//bmeta=bmeta+1;	
		}
	}
	else
	{
		//uint64_t begin = 0;
		//block *end_segment;
		bmeta = (block *)syscall_1(SYS_brk, begin);
		/*__asm__ __volatile__(
        	 "syscall"
        	 : "=a" (bmeta)
       		 : "a"(SYS_brk), "D"(begin)
		);*/
		if(bmeta==NULL)
			printf("starting pointer null\n");

		size_to_allocate=(uint64_t)bmeta;
		size_to_allocate+=(uint64_t)size+(uint64_t)sizeof(block);
		end_segment = (block *)syscall_1(SYS_brk, size_to_allocate);		
		/* __asm__ __volatile__(
                 "syscall"
                 : "=a" (end_segment)
                 : "a"(SYS_brk), "D"(size_to_allocate)
                );*/
	
		if((uint64_t)end_segment==(uint64_t)bmeta)
		{
			return NULL;
		}
		bmeta->available=FALSE;
		head=bmeta;
		bmeta->next=NULL;
		bmeta->size=size;

	}
	return bmeta+1;	
}




/*block *first_fit_block(int size)
{
	block *curr_ptr=head;
	while(curr_ptr!=NULL)
	{
		if(curr_ptr->size >=  size && curr_ptr->available == TRUE)
			return curr_ptr;
		curr_ptr=curr_ptr->next;	
	}	
	return NULL;
}*/

void free(void *ptr) 
{
	if (ptr==NULL) 
	    return;
  	ptr= (block*)ptr-1;
	((block*)ptr)->available = TRUE;
}
