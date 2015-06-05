.text
.global _irq0
_irq0:
	pushq %rax
	pushq %rbx
        pushq %rcx
        pushq %rdx
        pushq %rsi
        pushq %rdi
	pushq %rbp
        pushq %r8
        pushq %r9
        pushq %r10
        pushq %r11
        pushq %r12
        pushq %r13
        pushq %r14
        pushq %r15
        call timer_handler
        popq %r15
        popq %r14
        popq %r13
        popq %r12
        popq %r11
        popq %r10
        popq %r9
        popq %r8
	popq %rbp
        popq %rdi
        popq %rsi
        popq %rdx
        popq %rcx
        popq %rbx
        popq %rax
        iretq

.global _irq1
_irq1:
        pushq %rax
        pushq %rcx
        pushq %rdx
        pushq %rsi
        pushq %rdi
        pushq %r8
        pushq %r9
        pushq %r10
        pushq %r11
        movq %rsp,%rdi
        addq  $72, %rdi
        #pushq %rax
        call keyboard_handler
        #popq %rax
        popq %r11
        popq %r10
        popq %r9
        popq %r8
        popq %rdi
        popq %rsi
        popq %rdx
        popq %rcx
        popq %rax
        iretq

######### GPF ###############
.global _irq2  
_irq2:
      pushq %rax
      pushq %rcx
      pushq %rdx
      pushq %rsi  
      pushq %rdi
      pushq %r8
      pushq %r9
      pushq %r10
      pushq %r11
    
      call gpf_handler 

      popq %r11
      popq %r10
      popq %r9
      popq %r8
      popq %rdi
      popq %rsi 
      popq %rdx
      popq %rcx
      popq %rax 
      iretq


######### Page Fault ###############
.global _irq3 
_irq3:
      pushq %rax
      pushq %rbx
      pushq %rcx
      pushq %rdx
      pushq %rsi
      pushq %rdi
      pushq %r8
      pushq %r9
      pushq %r10
      pushq %r11
      call page_fault_handler 
      popq %r11
      popq %r10
      popq %r9
      popq %r8
      popq %rdi
      popq %rsi
      popq %rdx
      popq %rcx
      popq %rbx
      popq %rax   
      addq $0x8, %rsp
      iretq


######### SYSCALL ###############
.global _irq4
_irq4:
       pushq %rax
        pushq %rbx
        pushq %rcx
        pushq %rdx
        pushq %rsi
        pushq %rdi
	pushq %rbp
        pushq %r8
        pushq %r9
        pushq %r10
        pushq %r11
        pushq %r12
        pushq %r13
        pushq %r14
        pushq %r15
#	movq %rsp, %rdi
        call syscall_handler_isha
        popq %r15
        popq %r14
        popq %r13
        popq %r12
        popq %r11
        popq %r10
        popq %r9
        popq %r8
        popq %rbp
	popq %rdi
        popq %rsi
        popq %rdx
        popq %rcx
        popq %rbx
        popq %rax
        iretq

.global _irq5
_irq5:
        pushq %rax
        pushq %rbx
        pushq %rcx
        pushq %rdx
        pushq %rsi
        pushq %rdi
        pushq %rbp
        pushq %r8
        pushq %r9
        pushq %r10
        pushq %r11
        pushq %r12
        pushq %r13
        pushq %r14
        pushq %r15
        call divide_by_zero_handler
        popq %r15
        popq %r14
        popq %r13
        popq %r12
        popq %r11
        popq %r10
        popq %r9
        popq %r8
        popq %rbp
        popq %rdi
        popq %rsi
        popq %rdx
        popq %rcx
        popq %rbx
        popq %rax
        iretq

.global _call_asm_lidt
_call_asm_lidt:
	#lidt idtptr
	lidt (%rdi)
	ret 	
