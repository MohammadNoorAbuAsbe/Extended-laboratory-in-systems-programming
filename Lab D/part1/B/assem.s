extern malloc 
extern free
extern fgets
extern stdin
extern strlen
extern printf
global main

section .data
    numFormat db "%d", 10, 0         ; Format string for printing integers
    strFormat db "%s", 10, 0         ; Format string for printing strings
    hexFormat db "%02hhx", 0         ; Format string for printing hexadecimal values
    newLineFormat db 10, 0            ; Format string for printing a new line

section .data
    x_struct db 5                     ; Single-byte (8-bit) variable
    x_num db  0xaa, 1, 2, 0x44, 0x4f
    unsigned_int_var dd 0             ; Declaration of unsigned int variable

section .bss
    buffer resb 600             ;store my input
    saved_size: resd 1
    saved_struct: resd 1
section .text
main:
    push ebp
    mov ebp, esp
    pushad

    push dword [stdin]              ;fgets need 3 param
    push dword 600                   ;max lenght
    push dword buffer               ;input buffer
    call fgets
    add esp, 12                     ;remove 3 push from stuck

    ; Check if the first character of the buffer is a newline character
    cmp byte [buffer], 10  ; 10 represents the newline character '\n'
    je no_inputContinue


    push dword buffer        ; push the address of buffer onto the stack
    call strlen              ; call the strlen function
    add esp, 4               ; remove the buffer address from the stack

    mov ecx, eax
    shr eax, 1
    mov [saved_size], eax
    
    ; Size of the memory block to allocate (10 bytes)
    mov edi, eax
    mov ecx, 1      ; Number of times to shift left (multiplication factor)
    shl edi, cl     ; Shift the value in edx left by 1 bit
    push edi
    ; Call malloc function
    call malloc
    add esp, 4  ; Adjust the stack pointer
    ; Save the allocated memory address in saved_struct
    mov [saved_struct], eax

    ; Process the input buffer and save the data to x_num
    mov esi, buffer    ; Source buffer
    mov edi, saved_struct     ; Destination buffer
    mov ecx, dword[saved_size]       ; Length of the input buffer

    ; Adjust the destination pointer to the end of x_num array
    lea edi, [edi + ecx - 1]

    ; Iterate through the input buffer and convert each two hex digits to a number
convert_loop:
    cmp ecx, 0
    je continue
    mov al, [esi]     ; Load the first character
    inc esi

    sub al, '0'       ; Check if it's a number (0-9)
    cmp al, 9
    jbe convert_digit
    add al, '0'
    sub al, 'a'       ; Check if it's a letter (A-F)
    add al, 0xa

convert_digit:
    shl al, 4         ; Shift the value to the left to make room for the next digit

    mov ah, [esi]     ; Load the second character
    inc esi

    sub ah, '0'       ; Check if it's a number (0-9)
    cmp ah, 9
    jbe convert_digit2
    add ah, '0'
    sub ah, 'a'       ; Check if it's a letter (A-F)
    add ah, 0xa

convert_digit2:
    or al, ah         ; Combine the two values
    mov [edi], al     ; Save the combined value to x_num
    dec edi
    sub ecx, 1
    jmp convert_loop


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
