CC = clang
SO_PATH = /home/neil/fusor/build/array/libArrayPass.so
CC_FLAG = -Xclang -load -Xclang $(SO_PATH) -g

all: sha1 sha1_fusor

sha1: main.c sha1.c rtlib.c
	$(CC) main.c sha1.c rtlib.c -lpthread -o sha1

sha1_fusor: main.c sha1.c rtlib.c
	$(CC) $(CC_FLAG) main.c sha1.c rtlib.c -lpthread -o sha1_fusor

clean:
	- rm -rf *.o sha1 
