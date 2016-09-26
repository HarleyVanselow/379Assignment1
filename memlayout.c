#include <setjmp.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#define MEM_RW 0
#define MEM_RO 1
#define MEM_NO 2
#define PAGE_SIZE 4096

unsigned int current_memory_pointer = 0;
sigjmp_buf env;

struct memregion {
	void *from;
	void *to;
	unsigned char mode; /* MEM_RW, or MEM_RO, or MEM_NO */
};
void handle_seg_fault(int sig)
{
	printf("Encountered seg fault at %d\n",current_memory_pointer);
	current_memory_pointer+=PAGE_SIZE;
	siglongjmp(env,1);
}

int get_mem_layout (struct memregion *regions, unsigned int size)
{
	(void) signal(SIGSEGV,handle_seg_fault);
	do{
		// printf("Current memory %d\n", current_memory_pointer);
		sigsetjmp(env, 1);
		char* data = (char *) current_memory_pointer;
		int value = (int) data[0];
		printf("%d",value);
		current_memory_pointer+=PAGE_SIZE;
	}while(current_memory_pointer>0);
	
};
int get_mem_diff (struct memregion *regions, unsigned int howmany,
struct memregion *thediff, unsigned int diffsize);

int main(int argc, char const *argv[])
{
	get_mem_layout(NULL, 12);
	return 0;
}