EXE=translate
DEBUG=
WARNINGS=-Wall

$(EXE): main.c addresses tlb queue
	cc -O3 $(DEBUG) $(WARNINGS) -o $(EXE) $< addresses.o  pt_queue.o tlb.o

addresses: addresses.c addresses.h
	gcc -c $(DEBUG) $(WARNINGS) -o addresses.o addresses.c

tlb: tlb.c tlb.h addresses
	gcc -c $(DEBUG) $(WARNINGS) -o tlb.o tlb.c 

queue: pt_queue.c pt_queue.h addresses
	gcc -c $(DEBUG) $(WARNINGS) -o pt_queue.o pt_queue.c 

format:
	clang-format -style=file -i *.c *.h

clean:
	rm *.o || true; rm translate || true;