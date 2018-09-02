CC = clang
SO_PATH = /home/neil/fusor_new/build/array/libArrayPass.so
CC_FLAG = -Xclang -load -Xclang $(SO_PATH) -g

all: aes aes_fusor 

aes: aes.c
	$(CC) aes.c -o aes

aes_fusor: aes.c
	$(CC) $(CC_FLAG) aes.c -o aes_fusor

clean:
	rm -f aes aes_fusor
	rm -f aes_stdout aes_fusor_stdout
