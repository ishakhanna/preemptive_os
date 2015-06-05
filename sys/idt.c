#include <sys/sbunix.h>

#define IDT_MAX 256

void _irq0();
void _irq1();
void _irq2();
void _irq3();
void _irq4();
void _irq5();
struct idtdesc_t
{
	uint16_t offset_lo;
        uint16_t selector;
        uint64_t zero : 8;
        unsigned gate_type : 4;
        unsigned stor_seg : 1;  // 0 for interrupt gates
        unsigned dpl : 2;
        unsigned p : 1;
        uint16_t offset_mid;
        uint32_t offset_hi;
        uint32_t reserved;
}__attribute__((packed));

struct idtptr_t
{
	uint16_t max;
        uint64_t addr;
}__attribute__((packed));

struct idtdesc_t idt_table[IDT_MAX]; 
struct idtptr_t idtptr;

void _call_asm_lidt(struct idtptr_t *idtptr);

void remap_irq()
{
	outportb(0x20, 0x11);
    outportb(0xA0, 0x11);
    outportb(0x21, 0x20);
    outportb(0xA1, 0x28);
    outportb(0x21, 0x04);
    outportb(0xA1, 0x02);
    outportb(0x21, 0x01);
    outportb(0xA1, 0x01);
    outportb(0x21, 0x0);
    outportb(0xA1, 0x0);
}


void add_isr(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags, int priv)
{
	//printf("Argument passed: %p\n", base);
	idt_table[num].offset_lo= (base & 0xFFFF);	
	idt_table[num].offset_mid= (base>>16) & 0xFFFF;	
	idt_table[num].offset_hi= (base>>32);// & 0xFFFFFFFF;
	
	idt_table[num].selector = sel;
        idt_table[num].zero = 0;
	idt_table[num].gate_type = flags & 0xF;
        idt_table[num].stor_seg = (flags>>4) & 0x1;  
    //    idt_table[num].dpl = (flags>>1) & 0x11;
        idt_table[num].dpl = priv & 0x3;
        idt_table[num].p = (flags>>2) & 0x1;
}


void reload_idt()
{
	remap_irq();
	int size_table = (sizeof(struct idtdesc_t))*IDT_MAX;
	idtptr.addr= (uint64_t)&idt_table;
	idtptr.max= size_table - 1;
	add_isr(32, (uint64_t)_irq0, 0x08, 0x8E, 0);// timer
	add_isr(33,(uint64_t)_irq1,0x08,0x8E, 0);	//keyboard
	add_isr(13,(uint64_t)_irq2,0x08,0x8E, 0);	//gpf
	add_isr(14,(uint64_t)_irq3,0x08,0x8E, 0);	// page fault
	add_isr(128,(uint64_t)_irq4,0x08,0xAE, 3);	// syscall
	add_isr(0,(uint64_t)_irq5,0x08,0xAE, 3);        // divide_by_zero

	_call_asm_lidt(&idtptr);	
}
