section .data
    hello db 'hello world', 10 ; define the string "hello world" and a linefeed

section .text
    global main

main:
    ; write the string to stdout
    mov eax, 4      ; system call number for write
    mov ebx, 1      ; file descriptor for stdout
    mov ecx, hello  ; address of the string to print
    mov edx, 12     ; length of the string
    int 0x80        ; invoke the system call
    
    ; exit the program
    mov eax, 1      ; system call number for exit
    xor ebx, ebx    ; return 0 status
    int 0x80        ; invoke the system call