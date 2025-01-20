section .data
    newline: db 10 ; newline character

section .text
    extern strlen
    global main

main:
    push ebp
	mov ebp, esp;

    mov eax, DWORD [ebp + 8]    ; points to argc
    mov ebx, DWORD [ebp + 12]   ; points to argv
    mov ecx, 0      ; mov ZERO to count register

loop:
    cmp ecx,eax
    JE  exit      ; If yes, then jump to label exit

    push eax
    push ecx
    push ebx

    mov ecx, DWORD [ebx + ecx * 4] ;size = 4

    push ecx
    call strlen ; saved in eax
    pop ecx

    mov edx, eax; load length of first argument into edx
    mov eax, 4 ; load system call number for write into eax
    mov ebx, 1; load file descriptor for stdout into ebx
    int 0x80; invoke the system call

    mov ecx, newline
    mov edx, 1
    mov eax, 4
    mov ebx, 1
    int 0x80

    pop ebx
    pop ecx
    pop eax
    inc ecx
    jmp loop


exit:
    ; exit program
    mov eax, 1
    xor ebx, ebx
    int 0x80

























; main:
;     push ebp
; 	mov ebp, esp;

;     mov eax, DWORD [ebp + 8]    ; points to argc
;     mov ebx, DWORD [ebp + 12]   ; points to argv
;     mov ecx, 0      ; mov ZERO to count register

;     mov ecx, DWORD [ebx +8] ;size = 4

;     mov eax, 4 ; load system call number for write into eax
;     mov ebx, 1; load file descriptor for stdout into ebx
;     mov edx, 2; load length of first argument into edx
;     int 0x80; invoke the system call
    


;     ; exit program
;     mov eax, 1
;     xor ebx, ebx
;     int 0x80