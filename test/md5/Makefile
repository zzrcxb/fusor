CC = clang
SO_PATH = ../../build/fusor/libFusorPass.so
CC_FLAG = -Xclang -load -Xclang $(SO_PATH)

all: md5 md5_fusor

md5: md5.o 
	@$(CC) md5.o -o md5 && rm md5.o

md5.o: md5.c
	@$(CC) -Wall -DTEST_VECTORS -c $<

md5_fusor: md5.o_fusor 
	@$(CC)  md5.o -o fusor && rm md5.o

md5.o_fusor: md5.c
	@$(CC) $(CC_FLAG) -Wall -DTEST_VECTORS -c $<

clean:
	@rm -f md5 fusor
	@rm -f md5_stdout fusor_stdout
	@rm -f *.ll *.bc *.s *.o *.out *.txt
