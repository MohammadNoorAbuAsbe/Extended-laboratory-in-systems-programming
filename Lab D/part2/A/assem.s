
global main

section .data
    x_struct db 5
    x_num db 0xaa, 1, 2, 0x44, 0x4f
    y_struct db 6
    y_num db 0xaa, 1, 2, 3, 0x44, 0x4f

section .text
main:
    ; Initialize registers
    xor eax, eax
    xor ebx, ebx

    ; Load lengths of structures
    mov al, byte [x_struct]
    mov bl, byte [y_struct]

    ; Compare lengths
    cmp al, bl
    jge xGreater

yGreater:
    mov ebx, x_num
    mov eax, y_num
    jmp exit
    
xGreater:
    mov eax, x_num
    mov ebx, y_num
    jmp exit


exit:
    ; Exit the program
    xor eax, eax
    ret

