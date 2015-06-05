#include <sys/sbunix.h>
#include <sys/gdt.h>
#include <sys/idt.h>

char *shell_bin = "bin/sbush";
void * setup_page_tables(uint64_t kern_physbase, uint64_t kern_physfree);
extern uint64_t *table_base_ptr;
extern void *kern_max;
extern uint64_t *pte;
void vmmgr_init();
void free_page(char* virt_add, uint64_t phys_add);
volatile char global_buf[BUF_SIZE] = {0};
volatile uint16_t global_buf_index = 0;
volatile uint16_t reading = 0;
volatile char circular_buffer[CIRCULAR_BUF_SIZE] = {0};
volatile uint16_t read_ptr = 0;
volatile uint16_t write_ptr = 0;
volatile uint16_t has_bytes_to_write = 1;

volatile int newline = 0;
volatile uint64_t kmalloc_bump_ptr = 0xFFFFFFFF80000000 + (uint64_t)0x1000;
volatile uint64_t vm_allocated_pages = 0;

/* Free list head */
volatile block * head = NULL;

void start(uint32_t* modulep, void* physbase, void* physfree)
{
	struct smap_t {
		uint64_t base, length;
		uint32_t type;
	}__attribute__((packed)) *smap;
	while(modulep[0] != 0x9001) modulep += modulep[1]+2;
	int i=0;
	bitmap_init(physfree);
	for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
		if (smap->type == 1 /* memory */ && smap->length != 0) {
//			printf("Available Physical Memory [%x-%x]\n", smap->base, smap->base + smap->length);
		        mem_track_init(smap->base, smap->length, physfree);
			i++;
		}
	}
//	printf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
//	printf("Physbase and Physfree is: %p - %p\n", physbase, physfree);
	// kernel starts here
	set_kernel_video_page();
	kmalloc_bump_ptr += (uint64_t)kern_max;
	init_tarfs();
	init_process();

	printf("After init proc\n");
	printf("BACK IN MAIN\n");
	while(1);
}

#define INITIAL_STACK_SIZE 4096
char stack[INITIAL_STACK_SIZE];
uint32_t* loader_stack;
extern char kernmem, physbase;
struct tss_t tss;

void boot(void)
{
	// note: function changes rsp, local stack variables can't be practically used
//	register char *s;, *v;
	__asm__(
		"movq %%rsp, %0;"
		"movq %1, %%rsp;"
		:"=g"(loader_stack)
		:"r"(&stack[INITIAL_STACK_SIZE])
	);
	reload_gdt();
	setup_tss();
	reload_idt();
	__asm__ __volatile__ ("sti");
	start(
		(uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
		&physbase,
		(void*)(uint64_t)loader_stack[4]
	);
//	s = "!!!!! start() returned !!!!!";
//	for(v = (char*)0xb8000; *s; ++s, v += 2) *v = *s;
	while(1);
}
