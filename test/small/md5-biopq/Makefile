CC = ~/BiOpq/build/bin/clang

all: md5 

md5: md5.o rtlib.o
	$(CC) md5.o rtlib.o -lpthread -o md5

md5.o: md5.c
	#$(CC) md5.c -o md5.o
	$(CC) -Wall -DTEST_VECTORS -c $< -mllvm -bcf -mllvm -boguscf-prob=6 -mllvm -OpqType=5 -mllvm -OpqNum=10

rtlib.o: rtlib.c
	$(CC) -c rtlib.c -o rtlib.o
	
clean:
	- rm -rf *.o md5 
