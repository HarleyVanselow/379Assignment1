#include <setjmp.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "memlayout.h"

uint32_t current_memory_pointer;
sigjmp_buf env;
int mode_try = 2;

void handle_seg_fault(int sig)
{
	// printf("Encountered seg fault at %ld\n",(long)current_memory_pointer);
	mode_try--;
	siglongjmp(env,1);
}
// uint32_t min(uint32_t a, uint32_t b){
// 	if (a < b){
// 		return a;
// 	}
// 	return b;
// }

// uint32_t max(uint32_t a, uint32_t b){
// 	if (a > b){
// 		return a;
// 	}
// 	return b;
// }

int get_mem_layout (struct memregion *regions, unsigned int size)
{ 	
	current_memory_pointer= 0x0;
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

		// if (current_memory_pointer < 0) {break;}

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
			*((uint32_t *)region.to) = (current_memory_pointer);
			region.mode = prev_mode;
			// printf("start: %u, end: %u mode: %u\n", start_address, current_memory_pointer - PAGE_SIZE, prev_mode );
			// printf("start: %u, end: %u mode: %u\n", *(uint32_t *)region.from, *(uint32_t*)region.to, region.mode);

			regions[region_counter] = region;
			region_counter++;
			prev_mode=current_mode;
			start_address=current_memory_pointer;
		}
		// printf("Mem pointer:%u\n",current_memory_pointer);
		current_memory_pointer+=PAGE_SIZE;
		mode_try =2;
	}while(current_memory_pointer<4294967295-PAGE_SIZE);
	printf("Current pointer:%u\n",current_memory_pointer);
	
	struct memregion region;
	region.from = malloc(sizeof(uint32_t));
	region.to = malloc(sizeof(uint32_t));
	*((uint32_t *)region.from) = start_address;
	*((uint32_t *)region.to) = (current_memory_pointer);
	region.mode = prev_mode;
	regions[region_counter] = region;
	region_counter++;
	return region_counter;
};

int get_mem_diff (struct memregion *regions, unsigned int howmany,
	struct memregion *thediff, unsigned int diffsize)
{
	struct memregion *new_memregion = (struct memregion*)malloc(sizeof(struct memregion)*howmany);
	int new_number_of_regions = get_mem_layout(new_memregion,howmany);

	printf("Number of regions in new: %d\n", new_number_of_regions);
	int i;
	for (i =0; i < new_number_of_regions; i++)
	{
		int mode = new_memregion[i].mode;
		char * mode_text;
		if (mode == 0){
			mode_text = "RW";
		} else if (mode == 1){
			mode_text = "RO";
		} else {
			mode_text = "NO";
		}
		printf("0x%08X-0x%08X %s\n", *(uint32_t *)new_memregion[i].from, *(uint32_t*)new_memregion[i].to, mode_text);
	}


	int region_counter =0;

	int old_counter = 0;
	int new_counter = 0;
	// printf("diffs: %d, regions: %d\n", howmany, new_number_of_regions);
	int diff_counter=0;

	while (old_counter < howmany && new_counter < new_number_of_regions){
		struct memregion current_new_region = new_memregion[new_counter];
		struct memregion current_old_region = regions[old_counter];
			// printf("new_counter: %d, old_counter: %d\n", new_counter, old_counter);
			// printf("Old Region: 0x%08X-0x%08X %d\n", *(uint32_t *)current_old_region.from, *(uint32_t*)current_old_region.to, current_old_region.mode);
			// printf("New Region: 0x%08X-0x%08X %d\n", *(uint32_t *)current_new_region.from, *(uint32_t*)current_new_region.to, current_new_region.mode);
		if(current_new_region.mode != current_old_region.mode)
		{

			if (diff_counter > 0 && thediff[diff_counter-1].mode == current_new_region.mode
				&&*((uint32_t *)thediff[diff_counter-1].to) + PAGE_SIZE>= *((uint32_t *)current_new_region.from)){
				*((uint32_t *)thediff[diff_counter-1].to) = (uint32_t)current_new_region.to;
			} else {
				struct memregion diff_region;
				diff_region.from = malloc(sizeof(uint32_t));
				diff_region.to = malloc(sizeof(uint32_t));
				uint32_t new_from = *(uint32_t*)current_new_region.from;
				uint32_t new_to = *(uint32_t*)current_new_region.to;
				uint32_t old_from =*(uint32_t*)current_old_region.from;
				uint32_t old_to = *(uint32_t*)current_old_region.to;

// *((uint32_t *)diff_region.from) = max(*(uint32_t*)current_new_region.from, *(uint32_t*)current_old_region.from);
				if(new_from>=old_from){
					*((uint32_t *)diff_region.from) = new_from;
				}else{*((uint32_t *)diff_region.from) = old_from;}

// *((uint32_t *)diff_region.to) =  min(*(uint32_t*)current_new_region.to, *(uint32_t*)current_old_region.to);
				if(new_to<=old_to){
				*((uint32_t *)diff_region.to) = new_to;	
				}else{*((uint32_t *)diff_region.to) = old_to;}

				
				diff_region.mode = current_new_region.mode;
				if (diff_counter < diffsize){
					thediff[diff_counter] = diff_region;
				}
				diff_counter ++;
			}
		} 
		if (*((uint32_t *)current_old_region.to) >= *((uint32_t *)current_new_region.from) && *((uint32_t *)current_old_region.to )<= *((uint32_t *)current_new_region.to)){
			old_counter++;
		}

		if (*((uint32_t *)current_new_region.to) >= *((uint32_t *)current_old_region.from) && *((uint32_t *)current_new_region.to) <= *((uint32_t *)current_old_region.to)){
			new_counter++;
		}

	}
	return diff_counter;
}

