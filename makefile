all: memlayout.out

memlayout.out: memlayout.c
	gcc memlayout.c -o memlayout.out -m32 -static #-std=c99

run: memlayout.out
	./memlayout.out
clean:	
	rm *.out
	