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
		//printf("firstfitblock\n");
                if(curr_ptr->size >=  size && curr_ptr->available == TRUE)
                        return curr_ptr;
                curr_ptr=curr_ptr->next;
        }
        return NULL;
}


void *malloc(size_t size)
{
	//printf("Inside my Malloc\n");
	block *bmeta;
	uint64_t begin = 0;
	block *end_segment;
	uint64_t size_to_allocate=0;
	if(size<= 0)
		return NULL;
	if(head!= NULL)
	{
		//printf("Head is not null\n");
		bmeta=first_fit_block(size);
		if(bmeta!= NULL)
		{
			//printf("Not calling OS for memory");
			bmeta->available=FALSE;
			//bmeta=bmeta+1;
		}
		else
		{
			//printf("Asking OS for space\n");
			// ask OS for space

			__asm__ __volatile__(
        	 	"syscall"
        	 	: "=a" (bmeta)
	       		: "a"(SYS_brk), "D"(begin)
			);

			size_to_allocate=(uint64_t)bmeta;
			size_to_allocate+=(uint64_t)size+(uint64_t)sizeof(block);
			 __asm__ __volatile__(
                 	"syscall"
                 	: "=a" (end_segment)
                 	: "a"(SYS_brk), "D"(size_to_allocate)
                	);
	
		

		if((uint64_t)end_segment==(uint64_t)bmeta)
		{	
			//printf("syscall return same pointer\n");
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
//		printf("First allocation\n");
		__asm__ __volatile__(
        	 "syscall"
        	 : "=a" (bmeta)
       		 : "a"(SYS_brk), "D"(begin)
		);
		//printf("%d\n",bmeta);
		if(bmeta==NULL)
			printf("starting pointer null\n");

		size_to_allocate=(uint64_t)bmeta;
		size_to_allocate+=(uint64_t)size+(uint64_t)sizeof(block);
		 __asm__ __volatile__(
                 "syscall"
                 : "=a" (end_segment)
                 : "a"(SYS_brk), "D"(size_to_allocate)
                );
		//printf("%d\n",size_to_allocate);
		//printf("%d\n",end_segment);
	
		//printf("bmeta is:%d\n", (uint64_t)bmeta);
		//printf("end_segment is:%d\n", (uint64_t)end_segment);

		if((uint64_t)end_segment==(uint64_t)bmeta)
		{	
//			printf("syscall return same pointer\n");
			return NULL;
		}
//		printf("after allocating one block\n");
		bmeta->available=FALSE;
		head=bmeta;
//		printf("CHECKING BMETA LINE 115\n");
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
	//printf("\nFREE");
	if (ptr==NULL) 
	    return;
  	ptr= (block*)ptr-1;
	((block*)ptr)->available = TRUE;
}
