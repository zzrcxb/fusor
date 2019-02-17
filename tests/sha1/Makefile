CC = clang
SO_PATH = ../../build/fusor/libFusorPass.so
CC_FLAG = -Xclang -load -Xclang $(SO_PATH)

all: sha1 sha1_fusor

sha1: main.c sha1.c rtlib.c
	@$(CC) main.c sha1.c rtlib.c -lpthread -o sha1

sha1_fusor: main.c sha1.c rtlib.c
	@$(CC) $(CC_FLAG) main.c sha1.c rtlib.c -lpthread -o fusor

clean:
	@rm -f sha1 fusor
	@rm -f sha1_stdout fusor_stdout
	@rm -f *.ll *.bc *.o *.s *.out *.txt *.o-*
