pass:
	@cd build && cmake .. && make -j2

opt: rebuild
	@opt-3.8 -load build/fusor/libFusorPass.so -fusor example.bc | llvm-dis-3.8 | tee out.ll

assembly:
	@llc-3.8 out.ll && clang out.s

rebuild: example.c pass
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

clean:
	rm -f *.bc *.ll *.out *.s *.dot
