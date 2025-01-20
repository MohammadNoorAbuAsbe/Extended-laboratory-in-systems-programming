extern malloc
extern free

extern printf

global main

section .bss
    saved_size: resb 1
    saved_struct: resb 1
    argument resb 64

section .data
    numFormat db "%d", 10, 0         ; Format string for printing integers
    strFormat db "%s", 10, 0         ; Format string for printing strings
    hexFormat db "%02hhx", 0         ; Format string for printing hexadecimal values
    newLineFormat db 10, 0            ; Format string for printing a new line
    x_struct db 5
    x_num db 0x02, 1, 2, 0x44, 0x4f
    y_struct db 6
    y_num db 0x03, 1, 2, 3, 0x44, 0x4f
    max_len db 1
    min_len db 1
    unsigned_int_var dd 0             ; Declaration of unsigned int variable

section .text

allocate:
    ; Size of the memory block to allocate (10 bytes)
    mov al, byte [max_len]   ; Load max_len into AL
    mov [saved_size], al     ; Save the value in saved_size
    push edi
    ; Call malloc function
    call malloc
    add esp, 4  ; Adjust the stack pointer
    ; Save the allocated memory address in saved_struct
    mov [saved_struct], eax
    ret

getMinMax:
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
    mov byte [min_len], al
    mov byte [max_len], bl
    mov ebx, x_num
    mov eax, y_num
    ret
    
    xGreater:
    mov byte [min_len], bl   ; Store length of y_struct in min_len
    mov byte [max_len], al   ; Store length of x_struct in max_len
    mov ebx, y_num           ; Point ebx to x_num
    mov eax, x_num         ; Point eax to y_num
    ret            


add_multi:

    mov edi, saved_struct     ; Destination address for the result
    xor ecx, ecx              ; Clear the ecx register for loop count

loop:
    push ecx
    mov cl, [eax]             ; Load the first byte from the array in eax
    mov dl, [ebx]             ; Load the first byte from the array in ebx

    add dl, cl                ; Add the first byte from the array in ebx
    mov [edi], dl             ; Save the result in saved_struct
    pop ecx
    inc edi                   ; Move to the next byte in saved_struct
    inc eax                   ; Move to the next byte in the array in eax
    inc ebx                   ; Move to the next byte in the array in ebx

    inc ecx                   ; Increment the loop count
    cmp ecx, dword [min_len]   ; Compare the loop count with min_len
    jl loop                   ; Jump to loop if the loop count is less than min_len
    ret

continue:
    popad
    mov ebx, saved_struct
    mov ecx, saved_size

print_multi:
    mov al, [ecx]                ; Move the value of x_struct into the AL register (8-bit)
    movzx eax, al                     ; Zero-extend AL to EAX (32-bit)
    mov [unsigned_int_var], eax       ; Move the value from EAX to unsigned_int_var (32-bit)

    mov edi, [unsigned_int_var]       ; Load the value from unsigned_int_var into EDI for loop control
    mov esi, ebx                    ; Set the source index to the start of x_num array
    add esi, dword[unsigned_int_var]  ; Set the source index to the end of x_num array

    ArrayLoop:
    sub esi, 1                        ; Decrement the source index to move to the previous byte
    movzx eax, byte [esi]             ; Move the current byte from the x_num array into EAX
    push eax                          ; Push the current byte onto the stack
    push hexFormat                    ; Push the hexFormat format string onto the stack
    call printf                       ; Call printf to print the byte as a hexadecimal value
    cmp esi, ebx                    ; Compare the source index with the start of x_num array
    jne ArrayLoop                     ; Jump to ArrayLoop if the source index is not equal to the start

    EndLoop:
    push newLineFormat                ; Push the newLineFormat format string onto the stack
    call printf                       ; Call printf to print a new line

    mov eax, 0                        ; Set the exit code to 0
    mov esp, ebp
    pop ebp
    ret



no_inputContinue:
    popad
    mov ebx, x_num
    mov ecx, x_struct
    jmp print_multi

main:
    push ebp
    mov ebp, esp
    pushad

    mov eax, [esp+8]    ; Load the address of argv[1] into eax
    cmp byte [eax], '-' ; Compare the first character of argv[1] with '-'
    jne not_equal       ; Jump to not_equal if they are not equal
    cmp byte [eax+1], 'I' ; Compare the second character of argv[1] with 'I'
    jne not_equal       ; Jump to not_equal if they are not equal

    ; Code for when argv[1] is "-I"
    ; ...
    ;pushad
    not_equal:
    ; Code for when argv[1] is not "-I"
    ; ...
    
    call getMinMax
    push eax
    call allocate
    pop eax
    call add_multi
    jmp continue
    ;jmp no_inputContinue
    jmp exit

exit:
    ; Exit the program
    mov eax, 0                        ; Set the exit code to 0
    mov esp, ebp
    pop ebp
    ret
