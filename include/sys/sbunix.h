#ifndef __SBUNIX_H
#define __SBUNIX_H

#include <sys/defs.h>

void printf(const char *fmt, ...);
void write_on_console(char c);
void print_backspace();
void outportb(unsigned short _port, unsigned char _data);
void decrement_buffer_index();
void read_from_stdin(char *buffer, uint64_t bytes);
unsigned char inportb (unsigned short _port);

typedef uint64_t size_t;
typedef int64_t ssize_t;
typedef uint64_t off_t;
enum { O_RDONLY = 0, O_WRONLY = 1, O_RDWR = 2, O_CREAT = 0x40, O_DIRECTORY = 0x10000 };

/*----------------------CONSTANTS-------------------------------------*/
extern char *shell_bin;
#define MAX_OPEN_FILES 50
#define PATH_MAX 100
#define MAX_PID 3000 
#define BUF_SIZE 100
#define CIRCULAR_BUF_SIZE 4096
#define NAME_MAX 255

extern volatile char global_buf[BUF_SIZE];
extern volatile uint16_t global_buf_index;
extern volatile int newline;
extern volatile uint16_t reading;
extern volatile char circular_buffer[CIRCULAR_BUF_SIZE];
extern volatile uint16_t read_ptr;
extern volatile uint16_t write_ptr;
extern volatile uint16_t has_bytes_to_write;
/*--------------------STRUCTS--------------------------------------------*/
struct mm_struct{
    struct vm_area_struct *mmap;
//    uint64_t start_code, end_code, start_data, end_data;
};

struct dirent
{
        long d_ino;
        off_t d_off;
        unsigned short d_reclen;
        char d_name [NAME_MAX+1];
};

// VM Area Structure Definition
struct vm_area_struct
{
    struct mm_struct *vm_mm;
    uint64_t  start_add;
    uint64_t end_add;
    struct vm_area_struct *vma_next, *vma_prev;
    unsigned long vma_flags;
};

struct regs_sched
{
     uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rbp, rdi, rsi, rdx, rcx, rbx, rax, rip, cs, flags, rsp, ss; // Pushed by pushq i.e. all general purpose registers
};

// Process Control Block
typedef struct pcb_struct
{
    uint64_t pid;
    uint64_t ppid;
    uint64_t* u_stack;
    struct mm_struct *mm_st;
    uint64_t cr3;
    uint64_t brk_ptr;
    uint64_t sleep_time;
    uint64_t alarm_time;
    uint64_t rip; // Instruction Pointer Value
    uint64_t k_stack[1024];
//    uint16_t kernel_pml4_offset;
    uint64_t is_waiting_for;
    uint64_t file_descriptor[MAX_OPEN_FILES];
     char cur_working_dir[PATH_MAX];
    char * read_start;
    struct regs_sched s_regs;
    uint64_t f_pos[MAX_OPEN_FILES];
    uint64_t mapping[MAX_OPEN_FILES];
}pcb;

struct pcb_list {
  struct pcb_list *next;
  pcb *task;
};

struct file_entry
{
        char file_name[100];
        int reference_count; //0 represents free
        char * address; // address in tarfs
        char *file_type; // 0 = file; 1 = directory
        char * file_mode;
        char * file_size;
        char * user_id;
        char * group_id;
        int fd;
        int f_flags;
        int f_pos; // position of cursor

        struct file_entry * next;
        struct file_entry * prev;
};

/*-------------------MEMORY MANAGEMENT DEPENDENCIES-----------------------*/

#define MAX_CHUNK 4096   // Each chunk correspond to 8 pages
#define PAGE_SIZE 4096   // 4 KB page size
#define L4_SHIFT (39)
#define L3_SHIFT (30)
#define L2_SHIFT (21)
#define L1_SHIFT (12)
#define PR 0x1                  //Present
#define RW 0x2                  //Read-0/Write-1
#define US 0x4                  //User -1/Supervisor -0
#define PAGE_PHYSICAL_ADDRESS(x) (*x & 0xFFFFFFFFFFFFF000)
#define VIRTUAL_OFFSET 0xFFFFFFFF80000000
#define COW_MASK 0x0010000000000000
//#define COW_MASK 0x0008000000000000
#define KSTACK_INITIAL_PTR 1004
#define STACK_START 0xFFFFF00000000000
#define ALIGN_PAGE 0xFFFFFFFFFF000

typedef struct page_table{
    uint64_t entries[512];
}pt;

typedef struct page_directory_table{
    uint64_t entries[512];
}pd;

typedef struct page_directory_pointer_table{
    uint64_t entries[512];
}pdp;

typedef struct page_map_level_4{
    uint64_t entries[512];
}pml4;

uint64_t* get_free_page();
uint64_t get_pml4_offset(uint64_t virt_addr);
uint64_t get_pdp_offset(uint64_t virt_addr);
uint64_t get_pd_offset(uint64_t virt_addr);
uint64_t get_pt_offset(uint64_t virt_addr);
void page_look_up(uint64_t  virt_addr);
extern void write_cr3(uint64_t addr);
extern uint64_t read_cr3();
void  bitmap_init(void *physfree);
void set_bitmap(int page_no);
void clear_bitmap(int page_no);
void mem_track_init(uint64_t sbase, uint64_t slength, void *physfree);
void update_video_loc( uint64_t video_va);
void map_page(uint64_t phys, uint64_t virt);
void set_kernel_video_page();
void assign_page_with_self_ref(uint64_t virt_add);
void assign_page_cow(uint64_t virt_add);
int init_virtual_space_proc(pcb *proc);
uint64_t get_self_ref_add(uint64_t pml4_offset, uint64_t pdp_offset, uint64_t pd_offset, uint64_t pt_offset);
void delete_page_tables(pcb *process);
void free_frame(void *virt_add, uint64_t phys_add);
void incr_ref_count(uint64_t phys_add);
void decr_ref_count(uint64_t phys_add);
int get_ref_count(uint64_t phys_add);
uint64_t get_stack_end(pcb *proc);

/*-------------------PROCESS MANAGEMENT DEPENDENCIES-----------------------*/

#define VMA_X 0x1
#define VMA_W 0x2
#define VMA_R 0x4
#define VMA_MASKOS 0x00FF 0000
#define VMA_MASKPROC 0xFF00 0000
#define MAX_STACK_SIZE 8*1024*1024 // 8MB
#define STACK_SIZE 8192

extern pcb* current_process;
extern pcb* fg_process;
extern pcb* idle_process;

void init_file_desc(pcb * process);
void free_proc(pcb *proc);
void init_process();
uint64_t generate_pid();
int print_processes();
int kill_process(int pid);
void init_idle_process();
void init_heap(pcb *proc);
void claim_pid(int pid);
void init_stack(pcb *proc);
struct vm_area_struct *find_vma(struct vm_area_struct *mmap, uint64_t addr);
uint64_t extend_heap(pcb *proc, uint64_t new_brk_ptr);
int extend_stack(pcb *proc, uint64_t new_end_add);
void copy_vma(pcb *child, pcb *parent);
int copy_virtual_space(pcb *child, pcb *parent);
void print_vma(pcb *proc);
uint64_t _fork(pcb *proc);
int k_execve(char *filename, char **argv, char **env);
void decrement_alarm();
void copy_tarfs_stuff(pcb *child, pcb *parent);
/*-------------------MEMORY ALLOCATOR DEPENDENCIES-----------------------*/

/*
 * Whole virtual memory size is not tracked since it is so high.
 * (18,446,744,073,709,551,616) / (1024*1024*1024) GB
 */


#define VM_PAGE_SIZE (4 * 1024)		// Each page is 4 KB

typedef struct block_info
{
        int available;
        size_t size;
        struct block_info *next;
} block;

extern volatile uint64_t kmalloc_bump_ptr;
extern volatile uint64_t vm_allocated_pages;
//extern volatile struct block_info * head;

void * kmalloc1(size_t size);
void kfree1(void *ptr);
void * vm_get_free_pages(uint64_t);
uint64_t vm_get_base_address();

// remove remove_me method
void remove_me(void);

block * first_fit_block(size_t, block *);
void merge_free_list();
void kfree(void *);
block * create_free_list(block *, uint64_t);
void add_to_free_list(block *);

/*-------------------ELF DEPENDENCIES------------------------------------*/

// Reference:http://downloads.openwatcom.org/ftp/devel/docs/elf-64-gen.pdf
int load_process_tarfs(pcb *proc, char * filename);

typedef struct Elf64_Ehdr{
    unsigned char e_ident[16]; /* ELF identification */
    uint16_t e_type; /* Object file type */
    uint16_t e_machine; /* Machine type */
    uint32_t e_version; /* Object file version */
    uint64_t e_entry; /* Entry point address */
    uint64_t e_phoff; /* Program header offset */
    uint64_t e_shoff; /* Section header offset */
    uint32_t e_flags; /* Processor-specific flags */
    uint16_t e_ehsize; /* ELF header size */
    uint16_t e_phentsize; /* Size of program header entry */
    uint16_t e_phnum; /* Number of program header entries */
    uint16_t e_shentsize; /* Size of section header entry */
    uint16_t e_shnum; /* Number of section header entries */
    uint16_t e_shstrndx; /* Section name string table index */
}Elf64_Ehdr;

typedef struct Elf64_Phdr
{
    uint32_t p_type; /* Type of segment */
    uint32_t p_flags; /* Segment attributes */
    uint64_t p_offset; /* Offset in file */
    uint64_t p_vaddr; /* Virtual address in memory */
    uint64_t p_paddr; /* Reserved */
    uint64_t p_filesz; /* Size of segment in file */
    uint64_t p_memsz; /* Size of segment in memory */
    uint64_t p_align; /* Alignment of segment */
}Elf64_Phdr;

/*-----------------SCHEDULING------------------------------------*/

extern struct pcb_list *ready_queue;
extern struct pcb_list *wait_queue;
extern struct pcb_list *terminated_queue;

void schedule(struct regs_sched r);
void exit_process();
void print_list(struct pcb_list **list);
void insert_to_head_pcb_list(struct pcb_list **list, pcb *proc);
void insert_to_tail_pcb_list(struct pcb_list **list, pcb *proc);
void delete_in_pcb_list(struct pcb_list **list, pcb *proc);
int move_to_tail(struct pcb_list **list);
void context_switch();
void k_sleep(uint64_t time, struct regs_sched r);
int k_waitpid(int pid, struct regs_sched r);
void assign_foreground();
pcb *get_process_by_pid(int pid);

/*------------------------TARFS OPERATIONS--------------------------*/
void init_tarfs();
void display_all_files();
int tarfs_open(char * pathname, int flags, pcb *);
int tarfs_close(int fd);
void display_one_file();
int tarfs_read_file(int global_fd,char * buf, int bytes_to_read, pcb * process, int local_fd);
int get_free_fd(pcb *process);
uint64_t get_local_fd(pcb *process, uint64_t global_fd);
uint64_t get_global_fd(pcb *process, int local_fd);
void reset_local_fd(pcb * process, int local_fd);
int copy_current_directory(pcb * process, char * answer, int size);
int tarfs_change_directory(pcb * process, char * pathname);
int is_valid_dir(char * path);
int compare_path(char * tarfs_path, char * path);
int get_direct_local_fd(pcb *, int);
char * parse_filename(char * filename, pcb * process);

char * get_current_working_directory(pcb * process);
char* search_tarfs(char *filename);
int tarfs_lseek(int , pcb * current_process, int local_fd, int arg2, int arg3);
int duplicate_fd(pcb *, int);
int duplicate_fd_2(pcb *, int local_fd, int);
uint64_t tarfs_read_dir(int global_fd, char *,  uint64_t arg3, pcb * process, int local_fd);
int not_mapped(pcb * process, int stdin);

int read_from_circular_buffer(char * buf, int bytes_to_read);
char read_a_char_from_circular_buffer();
int write_to_circular_buffer(char c);
int difference();
int increment(int ptr);

void inc(int global_id);
void increment_file_desc(pcb * process);

/*------------------------MAIN BUFFER OPERATIONS--------------------*/
void write_to_buffer(char c);
char * get_buffer_position();
char read_from_buffer(char * pos);
void init_main_buffer();
void write_str_to_buffer(char *c);
int buffer_has_enter( char *);

/*-------------------------STRING UTILITIES--------------------------*/

char *ultohex(unsigned long num);
char *itoa(int num);
char *itohex(int num);
char *ltoa(uint64_t num);
void *memset(void *s, int c, uint64_t n);
void *memcpy(void *dest, void *src, uint64_t n);
int  atoi(char *str);
int oct_to_dec(int n);
char *strcpy(char *des, const char *src);
char *parse_execv_file(char *filename);
int strlen(const char *s);
int strcmp(const char *s1, const char *s2);
char *strncpy(char *des, const char *src, uint64_t n);

/*------------------------ERROR NUMBERS--------------------------------*/
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

#endif
