; ---------------------------------------------------------------------------------------- 
; Writes "Hello, World!" to the console using only system calls. Runs on 64-bit Linux only.
;
; To assemble and run:
; nasm -felf64 ex01_syscall.S && ld -o ex01_syscall ex01_syscall.o
;
; (c) Alexei Guirik, 2021
; This source is licensed under CC BY-NC 4.0 (https://creativecommons.org/licenses/by-nc/4.0/)
; ----------------------------------------------------------------------------------------

          global    _start

          section   .text
_start:   mov       rax, 1                  ; system call for write
          mov       rdi, 1                  ; file handle 1 is stdout
          mov       rsi, message            ; address of string to output
          mov       rdx, 14                 ; number of bytes
          syscall                           ; invoke operating system to do the write
          mov       rax, 60                 ; system call for exit
          xor       rdi, rdi                ; exit code 0
          syscall                           ; invoke operating system to exit

          section   .data
message:  db        "Hello, World!", 10      ; note the newline at the end
