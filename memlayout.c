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

uint32_t current_memory_pointer = 0x0;
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
	int region_counter =0;
	struct sigaction onSegFault;
	onSegFault.sa_handler = handle_seg_fault;
	sigemptyset(&onSegFault.sa_mask);
	onSegFault.sa_flags = 1;

	sigaction(SIGSEGV, &onSegFault, NULL);
	uint32_t start_address = current_memory_pointer;
	uint32_t prev_address;
	int current_mode = MEM_NO;
	int prev_mode = current_mode;
	
	do{
		sigsetjmp(env, 1);
	
		if (current_memory_pointer < 0) {break;}

		// printf("Current memory %ld\n", current_memory_pointer);
		uint32_t * data_pointer = ((uint32_t*) current_memory_pointer);
		
		if(mode_try == 2)
		{
			uint32_t data = * data_pointer;
			*data_pointer = data;
			// must be read and write
			current_mode = MEM_RW;
		// printf("Wrote at %lu\n", (long unsigned int) current_memory_pointer);

		} else if(mode_try ==1)
		{
			uint32_t data = * data_pointer;
			current_mode = MEM_RO;
		// printf("Read at %lu\n", (long unsigned int) current_memory_pointer);

		}else
		{
			current_mode = MEM_NO;
			// printf("couldn't read or write\n");
		}
		if(current_mode!=prev_mode)
		{
			struct memregion region;
			region.from = malloc(sizeof(uint32_t));
			region.to = malloc(sizeof(uint32_t));
			*((uint32_t *)region.from) = start_address;
			*((uint32_t *)region.to) = (current_memory_pointer-PAGE_SIZE);
			region.mode = prev_mode;
			// printf("start: %u, end: %u mode: %u\n", start_address, current_memory_pointer - PAGE_SIZE, prev_mode );
			// printf("start: %u, end: %u mode: %u\n", *(uint32_t *)region.from, *(uint32_t*)region.to, region.mode);

			regions[region_counter] = region;
			region_counter++;
			prev_mode=current_mode;
			start_address=current_memory_pointer;
		}
		current_memory_pointer+=PAGE_SIZE;
		mode_try =2;
	}while(current_memory_pointer>0);
	return region_counter;
};
int get_mem_diff (struct memregion *regions, unsigned int howmany,
struct memregion *thediff, unsigned int diffsize);

int main(int argc, char const *argv[])
{
	struct memregion * ptr;
	ptr = (struct memregion*)malloc(sizeof(struct memregion)*30);
	
	int number_of_regions = get_mem_layout(ptr, 30);
	int i = 0;
	for (i; i < number_of_regions; ++i)
	{
		printf("start: %u, end: %u mode: %u\n", *(uint32_t *)ptr[i].from, *(uint32_t*)ptr[i].to, ptr[i].mode);
	}
	
	return 0;
}