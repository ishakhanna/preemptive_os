.text

.global write_cr3
write_cr3:
    movq %rdi, %rax
    movq %rax, %cr3
    retq

.global read_cr3
read_cr3:
    movq %cr3, %rax
    retq