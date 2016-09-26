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

int32_t current_memory_pointer = 0x0;
sigjmp_buf env;

struct memregion {
	void *from;
	void *to;
	unsigned char mode; /* MEM_RW, or MEM_RO, or MEM_NO */
};

void handle_seg_fault(int sig)
{
	// printf("Encountered seg fault at %ld\n",(long)current_memory_pointer);
	current_memory_pointer+=PAGE_SIZE;
	siglongjmp(env,1);
}

int get_mem_layout (struct memregion *regions, unsigned int size)
{
	printf("%d",3);

	struct sigaction onSegFault;
	onSegFault.sa_handler = handle_seg_fault;
	sigemptyset(&onSegFault.sa_mask);
	onSegFault.sa_flags = 1;

	sigaction(SIGSEGV, &onSegFault, NULL);
	
	int mode = MEM_NO;
	do{
		if (current_memory_pointer < 0) {break;}
		sigsetjmp(env, 1);

		// printf("Current memory %ld\n", current_memory_pointer);
		char * data_pointer = ((char*) current_memory_pointer);
		
		
		
		char data = * data_pointer
		mode= MEM_RO;
		data_pointer = data;
		mode = MEM_RW;
		
		// printf("%c",value);
		// printf("no fault at %d\n", current_memory_pointer);
		printf("Read at %ld\n", (long) current_memory_pointer);
		current_memory_pointer+=PAGE_SIZE;
	}while(current_memory_pointer>0);
	
};
int get_mem_diff (struct memregion *regions, unsigned int howmany,
struct memregion *thediff, unsigned int diffsize);

int main(int argc, char const *argv[])
{
	printf("%d",1);
	get_mem_layout(NULL, 12);
	
	printf("%d",2);
	return 0;
}