global loader
extern kernelMain

MAGIC_NUMBER equ 0x1BADB002
FLAGS        equ 0x0
CHECKSUM     equ -(MAGIC_NUMBER + FLAGS)

section .text
    align 4
    dd MAGIC_NUMBER
    dd FLAGS
    dd CHECKSUM

loader:
    mov esp, kernel_stack + KERNEL_STACK_SIZE
    push ebx
    call kernelMain
    cli
    hlt

section .bss
    KERNEL_STACK_SIZE equ 4096
    align 4
    kernel_stack resb KERNEL_STACK_SIZE