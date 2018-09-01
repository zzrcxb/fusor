CC = ~/BiOpq/build/bin/clang

all: aes 

aes: aes.o rtlib.o
	$(CC) aes.o rtlib.o -lpthread -o aes

aes.o: aes.c
	#$(CC) aes.c -o aes.o
	$(CC) -Wall -DTEST_VECTORS -c $< -mllvm -bcf -mllvm -boguscf-prob=6 -mllvm -OpqType=5 -mllvm -OpqNum=10

rtlib.o: rtlib.c
	$(CC) -c rtlib.c -o rtlib.o
	
clean:
	- rm -rf *.o aes 
