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
	struct memregion region;
			region.from = malloc(sizeof(uint32_t));
			region.to = malloc(sizeof(uint32_t));
			*((uint32_t *)region.from) = start_address;
			*((uint32_t *)region.to) = (current_memory_pointer-PAGE_SIZE);
			region.mode = prev_mode;
	return ++region_counter;
};

int get_mem_diff (struct memregion *regions, unsigned int howmany,
struct memregion *thediff, unsigned int diffsize)
{
	struct memregion *new_memregion = (struct memregion*)malloc(sizeof(struct memregion)*sizeof);
	int new_number_of_regions = get_mem_layout(new_memregion,howmany);
	int region_counter;

	struct diff{
		unsigned char mode;
		uint32_t memory;
	}

	struct diff *diffs = (struct diff*)malloc(sizeof(struct diff)*((2**32)/PAGE_SIZE));

	struct memregion new = new_memregion[0];
	struct memregion old = regions[0];
	uint32_t try_memory=0;

	while(try_memory<=2**32)
	{
		int counter;
		int diff_counter;
		unsigned char old_mode;
		unsigned char new_mode;
		for(counter=0;counter<howmany;counter++)
		{
			if(try_memory>=regions[counter].from && try_memory<regions[counter].to)
			{
				old_mode = regions[counter].mode;
				break;
			}
		}

		for(counter=0;counter<new_number_of_regions;counter++)
		{
			if(try_memory>=regions[counter].from && try_memory<regions[counter].to)
			{
				new_mode = regions[counter].mode;
				break;
			}
		}

		if(new_mode!=old_mode)
		{
			struct diff new_diff;
			new_diff.mode = new_mode;
			new_diff.memory = try_memory;
			diffs[diff_counter] = new_diff;
			diff_counter++;
		}
		try_memory+=PAGE_SIZE;
	}

	diff_counter=0;
	uint32_t start_mem=0;
	unsigned char prev_diff_mode = diffs[0].mode;
	for(counter=0;counter<sizeof(diffs)/sizeof(diffs[0]);counter++)
	{
		unsigned char current_diff_mode = diffs[counter].mode;
		if(current_diff_mode != prev_diff_mode)
		{
			struct memregion diff;
			diff.from =start_mem;
			diff.to = thediff[counter].memory - PAGE_SIZE;
			diff.mode = prev_mode;
			thediff[diff_counter] = diff;
			diff_counter++;
		}
		prev_mode=current_mode;
	}
	return sizeof(thediff)/sizeof(thediff[0]);
}

