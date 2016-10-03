#include "memlayout.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>

int main(int argc, char const *argv[])
{
	struct memregion * ptr;
	ptr = (struct memregion*)malloc(sizeof(struct memregion)*NUMBER_REGIONS_GUESS);
	uint32_t address = 0;
	int number_of_regions_diff;
	struct memregion * diffptr;
	diffptr = (struct memregion*)malloc(sizeof(struct memregion)*NUMBER_REGIONS_GUESS);

	void* selected_to;
	void* selected_from;
	int i = 0;

	// Some action here
	int created_region_size = PAGE_SIZE*sizeof(void*) * 100;
	void* address_changed = mmap(NULL,created_region_size,PROT_READ, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	int number_of_regions = get_mem_layout(ptr, NUMBER_REGIONS_GUESS);
	printf("Number of regions: %d\n", number_of_regions);
	for (i =0; i < number_of_regions; i++)
	{
		int mode = ptr[i].mode;
		char const * mode_text;
		if (mode == 0){
			mode_text = "RW";
		} else if (mode == 1){
			mode_text = "RO";
		} else {
			mode_text = "NO";
		}
		printf("0x%08X-0x%08X %s\n", (uint32_t)ptr[i].from, (uint32_t)ptr[i].to, mode_text);
	}
	for(i=0;i<number_of_regions;i++)
	{
		if(ptr[i].mode == 1)
		{
			selected_to=ptr[i].to;
			selected_from=ptr[i].from;
			break;
		}
	}
	mmap(address_changed,created_region_size, PROT_WRITE|PROT_READ, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0);
	number_of_regions_diff = get_mem_diff(ptr, number_of_regions, diffptr, NUMBER_REGIONS_GUESS);
	
	printf("Number of diffs: %d\n", number_of_regions_diff);
	for (i =0; i < number_of_regions_diff; i++)
	{
		int mode = diffptr[i].mode;
		char * mode_text;
		if (mode == 0){
			mode_text = "RW";
		} else if (mode == 1){
			mode_text = "RO";
		} else {
			mode_text = "NO";
		}
		printf("0x%08X-0x%08X %s\n", (uint32_t)diffptr[i].from, (uint32_t)diffptr[i].to, mode_text);
	}
	return 0;
}