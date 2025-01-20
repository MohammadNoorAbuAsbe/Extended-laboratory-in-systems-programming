section .data
    format db "%d", 10, 0  ; format string for printf
    newline db 10, 0      ; newline character for printing

section .text
global main

extern printf, puts
global _printf, _puts

main:
    ; Set up the stack frame
    push ebp
    mov ebp, esp

    ; Print argc in decimal format
    mov eax, dword [ebp + 8]  ; Get argc value from stack
    push eax                 ; Push argc as argument for printf
    push format              ; Push format string as argument for printf
    call printf              ; Call printf function
    add esp, 8               ; Clean up the stack after the function call

    ; Print argv[i] for all i from 0 to argc-1
    mov ebx, dword [ebp + 12]  ; Get argv pointer from stack
    mov eax, dword [ebp + 8]   ; Get argc value from stack

    cmp eax, 1                ; Compare argc with 1 (to handle case when argc is 0)
    jle exit_loop             ; If argc is 0 or 1, exit the loop

    xor ecx, ecx              ; Clear ecx to use it as a counter

print_loop:
    push ecx
    push eax
    cmp ecx, eax              ; Compare ecx with eax to check if loop should end
    jge exit_loop             ; If ecx is greater than or equal to eax, exit the loop

    mov edx, dword [ebx + ecx*4]  ; Get argv[i] value (address of the string)
    push edx                      ; Push argv[i] as argument for puts
    call puts                     ; Call puts function
    add esp, 4                    ; Clean up the stack after the function call
    pop eax
    pop ecx
    inc ecx                       ; Increment ecx to iterate over argv values

    jmp print_loop                ; Jump back to print_loop

exit_loop:
    ; Clean up the stack and exit the program
    mov esp, ebp
    pop ebp
    xor eax, eax             ; Return 0 status code
    ret
