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

		
	int i = 0;
	int number_of_regions = get_mem_layout(ptr, NUMBER_REGIONS_GUESS);
	printf("Number of regions: %d\n", number_of_regions);
	for (i =0; i < number_of_regions; i++)
	{
		int mode = ptr[i].mode;
		char const* mode_text;
		if (mode == 0){
			mode_text = "RW";
		} else if (mode == 1){
			mode_text = "RO";
		} else {
			mode_text = "NO";
		}
		printf("0x%08X-0x%08X %s\n", (uint32_t)ptr[i].from, (uint32_t)ptr[i].to, mode_text);
	}
 	//Grow the heap
	void* test_mem = malloc(PAGE_SIZE*sizeof(void*)*4096);
	number_of_regions_diff = get_mem_diff(ptr, number_of_regions, diffptr, NUMBER_REGIONS_GUESS);
	free(test_mem);
	printf("Number of diffs: %d\n", number_of_regions_diff);
	for (i =0; i < number_of_regions_diff; i++)
	{
		int mode = diffptr[i].mode;
		char const* mode_text;
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