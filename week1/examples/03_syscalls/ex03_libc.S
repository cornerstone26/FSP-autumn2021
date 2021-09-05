; ---------------------------------------------------------------------------------------- 
; Writes "Hello, World!" to the console using libc calls. Runs on 64-bit Linux only.
;
; To assemble and run:
; nasm -felf64 ex03_libc.S && ld -dynamic-linker /lib64/ld-linux-x86-64.so.2 -o ex03_libc ex03_libc.o -lc
;
; (c) Alexei Guirik, 2021
; This source is licensed under CC BY-NC 4.0 (https://creativecommons.org/licenses/by-nc/4.0/)
; ----------------------------------------------------------------------------------------

        section .data

        fmt     db `%s\n`, 0
        hello   db 'Hello, world!', 0

        global    _start
        extern printf, exit

        section   .text
        
_start: mov rdi, fmt
        mov rsi, hello
        xor eax, eax
        call printf

        xor edi, edi
        call exit
