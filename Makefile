pass:
	@cd build && cmake .. && make -j2

opt: rebuild
	@opt -load build/fusor/libFusorPass.so -fusor example.ll | llvm-dis | tee out.ll

cfg: opt
	@opt -dot-cfg out.ll > /dev/null

assembly:
	@llc out.ll && clang out.s

rebuild: example.c pass
	@clang -S -emit-llvm example.c
	@clang -c -emit-llvm example.c

full: pass example.c
	@clang -Xclang -load -Xclang build/fusor/libFusorPass.so example.c

test: pass
	@for dir in $$(ls tests); do \
		echo "\n================ Testing $$dir ================"; \
		cd tests/$$dir && make >/dev/null 2>/dev/null; \
		./$$dir > $$dir.txt && ./fusor > fusor.txt;\
		if [ $$(diff $$dir.txt fusor.txt | wc -c) != 0 ]; then \
			echo "---------------- $$dir FAIL ----------------";\
		else \
			echo "---------------- $$dir PASS ----------------";\
		fi; \
		make clean >/dev/null 2>/dev/null; \
		cd ../..; \
	done;

clean:
	rm -f *.bc *.ll *.out *.s *.dot

debug: log.c pass example.c
	@clang -c log.c
	@clang -Xclang -load -Xclang build/fusor/libFusorPass.so -c example.c
	@clang log.o example.o
	@rm example.o
