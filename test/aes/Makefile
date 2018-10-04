CC = clang
SO_PATH = ../../build/fusor/libFusorPass.so
CC_FLAG = -Xclang -load -Xclang $(SO_PATH)

all: aes aes_fusor 

aes: aes.c
	@$(CC) aes.c -o aes

aes_fusor: aes.c
	@$(CC) $(CC_FLAG) aes.c -o fusor

clean:
	@rm -f aes fusor
	@rm -f aes_stdout fusor_stdout
	@rm -f *.ll *.bc *.s *.o *.out *.txt
