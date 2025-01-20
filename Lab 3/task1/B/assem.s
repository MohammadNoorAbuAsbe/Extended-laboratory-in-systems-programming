section .data
    Infile equ 0 ; stdin file descriptor
    Outfile equ 1 ; stdout file descriptor
    newline: db 10 ; newline character
    buffer_size equ 1024 ; size of the buffer used for storing user input

    prompt db "Enter a string: ", 0 ; message to prompt user for input

section .bss
    buffer resb buffer_size ; reserve space in memory for buffer to store user input
    num resb 1000 ; reserve space in memory to store command line argument

section .text
    extern strlen ; declare external function strlen
    global main ; define main function as global

main:
    push ebp ; save the value of the base pointer
	mov ebp, esp ; set the value of the base pointer to the current value of the stack pointer

    mov eax, DWORD [ebp + 8] ; get the value of argc
    mov ebx, DWORD [ebp + 12] ; get the value of argv
    mov edx, 0 ; set the pointer to the first argument in argv
    mov esi, eax ; set the value of esi to argc (used to loop through the arguments)
    push dword 0 ; set in stream to 0 (stdin)
    push dword 2 ; set out stream to 1 (stdout)
    push dword 0 ; no input file specified
    push dword 1 ; no output file specified

argvCheck:
    sub esi, 1 ; decrement the value of esi
    add edx, 4 ; increment the pointer to the next argument
	cmp esi, 0 ; check if all arguments have been processed
	jz checkInputType ; if all arguments have been processed, go to checkInputType
	mov ecx, dword[ebx+edx] ; get the value of the current argument

minusCheck:
	cmp byte[ecx], '-' ; check if the argument starts with a dash
	je iCheck ; if it does, go to iCheck
    jmp argvCheck ; if it doesn't, go back to argvCheck

iCheck:
    cmp byte[ecx+1], 'i' ; check if the argument specifies an input file
	je readInputFile ; if it does, go to readInputFile

oCheck:
    cmp byte[ecx+1], 'o' ; check if the argument specifies an output file
    je readOutFile ; if it does, go to readOutFile
    jmp argvCheck ; if it doesn't, go back to argvCheck

readInputFile:
    pushad ; save the values of all registers on the stack

    ; open input file
    mov ebx, ecx ; get the name of the input file from the argument
    add ebx, 2 ; move the pointer to the start of the file name
    mov eax, 0x05 ; set opcode for open syscall
    mov ecx, 0 ; set mode to read-only
    mov edx, 2 ; set flags to read and write
    int 0x80 ; call the system

    ; read from input file
    push eax ; save file descriptor for later use
    mov ebx, eax ; set file descriptor as parameter
    mov eax, 3 ; set opcode for read syscall
    mov ecx, buffer ; set address of buffer
    mov edx, buffer_size ; set maximum number of bytes to read
    int 0x80 ; call the system

    ; close input file
    mov eax, 0x06 ; set opcode for close syscall
    pop ebx ; get file descriptor
    int 0x80
    popad
    mov dword[esp+12],1
    jmp argvCheck

readOutFile:
    ; set output file flag to 1
    mov dword [esp+4], 1
    ; get file name from command line argument
    add ecx, 2
    mov [num], ecx
    sub ecx ,2
    jmp argvCheck

checkInputType:
    cmp dword[esp+12],0
    je userInput
    jmp fileInput


userInput:
    mov eax, 4       ; sys_write
    mov ebx, Outfile       ; stdout
    mov ecx, prompt  ; address of prompt string
    mov edx, 15      ; length of prompt string
    int 0x80

    ; read input
    mov eax, 3       ; sys_read
    mov ebx, Infile       ; stdin
    mov ecx, buffer  ; address of buffer
    mov edx, buffer_size ; maximum number of bytes to read
    int 0x80

fileInput:
    ; change the value of each byte in the buffer
    mov ebx, buffer  ; move the address of buffer to ebx
    mov ecx, 0       ; initialize the counter to 0
    push ebx
    call strlen
    pop ebx



    
encode:
    cmp ecx, eax     ; compare the counter with the buffer size
    jge checkoutputType ; if the counter is >= eax, cont
    cmp byte [ebx+ecx], 'A'
    jl not_alpha
    cmp byte [ebx+ecx], 'z'
    jg not_alpha
    mov al, [ebx+ecx] ; move the value of the byte at [ebx+ecx] to al
    add al, 1        ; add 1 to the value of al
    mov [ebx+ecx], al ; move the new value of al to [ebx+ecx]
    inc ecx           ; increment the counter
    jmp encode   ; loop back to the beginning of the loop

not_alpha:
    inc ecx           
    jmp encode


checkoutputType:
    mov byte [ebx+ecx], 0 ; add null byte to end of buffer
    cmp dword[esp+4],0
    je normalOutput
    jmp fileOutput

normalOutput:
    mov edx, eax
    mov eax, 4       ; sys_write
    mov ebx, Outfile ; stdout
    mov ecx, buffer  ; address of prompt string
    ;mov edx, buffer_size      ; length of prompt string
    int 0x80

fileOutput:
    mov ecx, [num]
    mov eax, 5 ; open syscall
    mov ebx, ecx ; filename
    mov ecx, 1 ; write-only mode
    mov edx, 0666 ; permissions
    int 0x80 ; call the system
    ; move file descriptor to ebx
    mov ebx, eax
    ; write buffer to output file
    mov ecx, buffer ; buffer
    push ecx
    call strlen
    pop ecx
    mov edx, eax
    mov eax, 4 ; write syscall
    ;mov edx, buffer_size ; buffer length
    int 0x80 ; call the system
    ; close output file
    mov eax, 6 ; close syscall
    mov ebx, dword [esp+8] ; file descriptor
    int 0x80 ; call the system
    ; jump back to argvCheck

exit:
    ; exit program
    mov eax, 1
    xor ebx, ebx
    int 0x80