CC = clang
SO_PATH = ../../../build/array/libArrayPass.so
CC_FLAG = -Xclang -load -Xclang $(SO_PATH)

all: rc4 rc4_fusor

rc4: rc4.c
	$(CC) rc4.c -o rc4 

rc4_fusor: rc4.c
	$(CC) $(CC_FLAG) rc4.c -o rc4_fusor 

clean:
	rm -f rc4 rc4_fusor
	rm -f rc4_stdout rc4_fusor_stdout
	rm -f *.ll *.o *.bc *.s *.out
