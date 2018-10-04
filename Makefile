pass:
	@cd build && cmake .. && make -j2

build:
	@mkdir build

opt: example.bc
	@opt-3.8 -load build/fusor/libFusorPass.so -array example.bc > /dev/null

rebuild: example.c
	@clang -S -emit-llvm example.c
	@clang -c -emit-llvm example.c

full: pass example.c
	@clang -Xclang -load -Xclang build/fusor/libFusorPass.so example.c

test: pass
	@for dir in $$(ls test); do \
		echo "\n================ Testing $$dir ================"; \
		cd test/$$dir && make >/dev/null 2>/dev/null; \
		./$$dir > $$dir.txt && ./fusor > fusor.txt;\
		if [ $$(diff $$dir.txt fusor.txt | wc -c) != 0 ]; then \
			echo "---------------- $$dir FAIL ----------------";\
		else \
			echo "---------------- $$dir PASS ----------------";\
		fi; \
		make clean >/dev/null 2>/dev/null; \
		cd ../..; \
	done;
