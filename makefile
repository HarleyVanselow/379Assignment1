all: mem_mod1.out mem_mod2.out mem_mod3.out

memlayout.o: memlayout.c
	gcc memlayout.c -c -o memlayout.o -m32 -static -I.

mem_mod1.out: memlayout.o
	gcc mem_mod1.c memlayout.o -o mem_mod1.out -m32

mem_mod2.out: memlayout.o
	gcc mem_mod2.c memlayout.o -o mem_mod2.out -m32

mem_mod3.out: memlayout.o
	gcc mem_mod3.c memlayout.o -o mem_mod3.out -m32

run1: mem_mod1.out
	./mem_mod1.out

run2: mem_mod2.out
	./mem_mod2.out

run3: mem_mod3.out
	./mem_mod3.out

clean:	
	rm *.out *.o
	
