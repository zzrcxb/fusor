CC = clang
SO_PATH = /home/neil/fusor_new/build/array/libArrayPass.so
CC_FLAG = -Xclang -load -Xclang $(SO_PATH) -g

all: sha1 sha1_fusor

sha1: main.c sha1.c rtlib.c
	$(CC) main.c sha1.c rtlib.c -lpthread -o sha1

sha1_fusor: main.c sha1.c rtlib.c
	$(CC) $(CC_FLAG) main.c sha1.c rtlib.c -lpthread -o sha1_fusor

test: main.c sha1.c rtlib.c
	$(CC) $(CC_FLAG) -c main.c
	$(CC) -c sha1.c
	$(CC) $(CC_FLAG) -c rtlib.c
	$(CC) main.o sha1.o rtlib.o -lpthread -o test

clean:
	- rm -rf *.o sha1 sha1_fusor *.ll *.bc *.s 
