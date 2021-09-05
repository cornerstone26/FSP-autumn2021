; ---------------------------------------------------------------------------------------- 
; Writes "Hello, World!" to the console using only legacy int 0x80. Runs on Linux only.
; Based on example from http://faculty.nps.edu/cseagle/assembly/sys_call.html
;
; To assemble and run:
; nasm -felf64 ex02_int80.S && ld -o ex02_int80 ex02_int80.o
;
; (c) Alexei Guirik, 2021
; This source is licensed under CC BY-NC 4.0 (https://creativecommons.org/licenses/by-nc/4.0/)
; ----------------------------------------------------------------------------------------

          global    _start

          section   .text
_start:   mov       rax, 4                  ; system call for write
          mov       rbx, 1                  ; file handle 1 is stdout
          mov       rcx, message            ; address of string to output
          mov       rdx, 14                 ; number of bytes
          int       0x80                    ; invoke operating system to do the write
          mov       rax, 1                  ; system call for exit
          mov       rbx, 42                 ; exit code 42
          int       0x80                    ; invoke operating system to exit

          section   .data
message:  db        "Hello, World!", 10      ; note the newline at the end
