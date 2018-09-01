CC = ~/BiOpq/build/bin/clang

OBJS=rtlib.o test.o

rc4:$(OBJS)
	$(CC) $(OBJS) -lpthread -o rc4 

test.o:test.c
	$(CC) -Wall -DTEST_VECTORS -c $< -mllvm -bcf -mllvm -boguscf-prob=6 -mllvm -OpqType=5 -mllvm -OpqNum=10

rtlib.o:rtlib.c
	$(CC) -c $<

clean:
	rm -rf *.o rc4 
