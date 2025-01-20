extern printf
extern malloc
global main

section .data
    numFormat db "%d", 10, 0         ; Format string for printing integers
    strFormat db "%s", 10, 0         ; Format string for printing strings
    hexFormat db "%02hhx", 0         ; Format string for printing hexadecimal values
    newLineFormat db 10, 0            ; Format string for printing a new line

section .data
    x_struct db 5                     ; Single-byte (8-bit) variable
    x_num db 0xaa, 1,2,0x44,0x4f      ; Example array of bytes
    unsigned_int_var dd 0             ; Declaration of unsigned int variable

section .text
main:
    push ebp
    mov ebp, esp

print_multi:
    mov al, [x_struct]                ; Move the value of x_struct into the AL register (8-bit)
    movzx eax, al                     ; Zero-extend AL to EAX (32-bit)
    mov [unsigned_int_var], eax       ; Move the value from EAX to unsigned_int_var (32-bit)

    mov edi, [unsigned_int_var]       ; Load the value from unsigned_int_var into EDI for loop control
    mov esi, x_num                    ; Set the source index to the start of x_num array
    add esi, dword[unsigned_int_var]  ; Set the source index to the end of x_num array

ArrayLoop:
    sub esi, 1                        ; Decrement the source index to move to the previous byte
    movzx eax, byte [esi]             ; Move the current byte from the x_num array into EAX
    push eax                          ; Push the current byte onto the stack
    push hexFormat                    ; Push the hexFormat format string onto the stack
    call printf                       ; Call printf to print the byte as a hexadecimal value
    cmp esi, x_num                    ; Compare the source index with the start of x_num array
    jne ArrayLoop                     ; Jump to ArrayLoop if the source index is not equal to the start

EndLoop:
    push newLineFormat                ; Push the newLineFormat format string onto the stack
    call printf                       ; Call printf to print a new line

    mov eax, 0                        ; Set the exit code to 0
    mov esp, ebp
    pop ebp
    ret