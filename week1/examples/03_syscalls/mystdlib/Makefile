.PHONY: all
all: mystdlib.o hello
clean:
	rm *.o hello
mystdlib.o: mystdlib.h mystdlib.c
	gcc -fno-builtin -c mystdlib.c
hello: hello.c mystdlib.o
	gcc -static -nostdlib -ffreestanding -fno-builtin -o hello hello.c mystdlib.o

