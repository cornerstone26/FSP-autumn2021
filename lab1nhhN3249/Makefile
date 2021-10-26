CFLAGS=-Wall -Wextra -Werror -O2
TARGETS=lab1test lab1nhhN3249 libnhhN3249.so

.PHONY: all clean

all: $(TARGETS)

clean:
	rm -rf *.o $(TARGETS)

lab1test: lab1test.c plugin_api.h
	gcc $(CFLAGS) -o lab1test lab1test.c -ldl

lab1nhhN3249: lab1nhhN3249.c plugin_api.h
	gcc $(CFLAGS) -o lab1nhhN3249 lab1nhhN3249.c -ldl

libnhhN3249.so: libnhhN3249.c plugin_api.h
	gcc $(CFLAGS) -shared -fPIC -o libnhhN3249.so libnhhN3249.c -ldl -lm


