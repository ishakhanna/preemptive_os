#include <sys/sbunix.h>

void *head;

block *first_fit(size_t size)
{
//	printf("In first fit:SIZe=%d\n ",size);
        block *curr_ptr=head;
        while(curr_ptr!=NULL)
        {
//		printf("Ptr=%p,  available=%d, size=%d\n",curr_ptr, curr_ptr->available, curr_ptr->size);
                if(curr_ptr->size >=  size && curr_ptr->available == 1){
                        curr_ptr->available = 0;
                        return curr_ptr;
                }

                curr_ptr=curr_ptr->next;
        }
        return NULL;
}

void *kmalloc1(size_t size1){
//	printf("-------In my new Kmalloc-------");
    block *add;
    size_t size = size1 + sizeof(struct block_info);
    if(head == NULL && ((kmalloc_bump_ptr+size) < 0xFFFFFFFFFFFFF000)){
//	printf("Isha size in head null=%d\n",size);
        uint64_t old_ptr = kmalloc_bump_ptr;
        kmalloc_bump_ptr += size;
        add = (block *)old_ptr;
        add->available = 0;
        add->size = size;
        add->next = NULL;

        head = (void *)add;
        return (void *)((uint64_t)add+sizeof(struct block_info));
    }
    else{

        add = first_fit(size);
	
//	printf("Isha size after first fit=%p,%p\n",add,size);

        if(add)
            return (void *)((uint64_t)add+sizeof(struct block_info));

        if((kmalloc_bump_ptr+size) < 0xFFFFFFFFFFFFF000){
            uint64_t old_ptr = kmalloc_bump_ptr;
            kmalloc_bump_ptr += size;
            add = (block *)old_ptr;
            add->available = 0;
            block *temp = head;
            head=add;
            add->next = temp;
            add->size = size;
//	printf("Not found in first fit=%p,%p\n",add,size);


            return (void *)((uint64_t)add+sizeof(struct block_info));
        }
    }

	return NULL;
}

void kfree1(void *ptr)
{
//	printf("------In kfree-----\n");
	if (ptr == NULL)
	    return;

  	block *ptr1 = (block *)((uint64_t)ptr - sizeof(struct block_info));
	ptr1->available = 1;
}

