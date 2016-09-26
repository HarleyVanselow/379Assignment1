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
int mode_try = 2;

struct memregion {
	void *from;
	void *to;
	unsigned char mode; /* MEM_RW, or MEM_RO, or MEM_NO */
};

void handle_seg_fault(int sig)
{
	// printf("Encountered seg fault at %ld\n",(long)current_memory_pointer);
	mode_try--;
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
	int32_t start_address = current_memory_pointer;
	int32_t prev_address;
	int current_mode = MEM_NO;
	int prev_mode = current_mode;
	
	do{
		sigsetjmp(env, 1);
	
		if (current_memory_pointer < 0) {break;}

		// printf("Current memory %ld\n", current_memory_pointer);
		char * data_pointer = ((char*) current_memory_pointer);
		
		if(mode_try == 2)
		{
			char data = * data_pointer;
			data_pointer = &data;
			// must be read and write
			current_mode = MEM_RW;
			//save
		printf("Wrote at %ld\n", (long) current_memory_pointer);

		}else if(mode_try ==1)
		{
			char data = * data_pointer;
			current_mode = MEM_RO;
		printf("Read at %ld\n", (long) current_memory_pointer);

		}else
		{
			current_mode = MEM_NO;
		}

		current_memory_pointer+=PAGE_SIZE;
		mode_try =2;
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