pass:
	@cd build && cmake .. && make -j2

test:
	@opt-3.8 -load build/array/libArrayPass.so -array example.bc > /dev/null

rebuild:
	clang -S -emit-llvm example.c
	clang -c -emit-llvm example.c

full: rebuild
	opt-3.8 -load build/array/libArrayPass.so -array example.bc > out.bc
	llc-3.8 out.bc
	clang out.s

build:
	opt-3.8 -load build/array/libArrayPass.so -array example.bc > out.bc
	llc-3.8 out.bc
	clang out.s
