CC = clang
SO_PATH = /home/neil/fusor_new/build/array/libArrayPass.so
CC_FLAG = -Xclang -load -Xclang $(SO_PATH)

all: hmac hmac_fusor

hmac: hmac_sha2.o sha2.o
	$(CC) sha2.o hmac_sha2.o -o hmac && rm *.o

hmac_sha2.o: hmac_sha2.c hmac_sha2.h
	$(CC) -Wall -DTEST_VECTORS -c hmac_sha2.c -o hmac_sha2.o

sha2.o: sha2.c sha2.h
	$(CC) -c sha2.c -o sha2.o


hmac_fusor: hmac_sha2.o_fusor sha2.o_fusor
	$(CC) sha2.o hmac_sha2.o -o hmac_fusor && rm *.o

hmac_sha2.o_fusor: hmac_sha2.c hmac_sha2.h
	$(CC) -Wall -DTEST_VECTORS -c hmac_sha2.c -o hmac_sha2.o

sha2.o_fusor: sha2.c sha2.h
	$(CC) $(CC_FLAG) -g -c sha2.c -o sha2.o

clean:
	- rm -rf *.o hmac hmac_fusor
