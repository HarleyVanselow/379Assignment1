#include "memlayout.h"
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char const *argv[])
{
	struct memregion * ptr;
	ptr = (struct memregion*)malloc(sizeof(struct memregion)*30);

	struct memregion * diffptr;
	diffptr = (struct memregion*)malloc(sizeof(struct memregion)*30);

	int number_of_regions = get_mem_layout(ptr, 30);
	int i;
	printf("Number of regions: %d\n", number_of_regions);
	for (i=0; i < number_of_regions; ++i)
	{
		int mode = ptr[i].mode;
		char * mode_text;
		if (mode == 0){
			mode_text = "RW";
		} else if (mode == 1){
			mode_text = "RO";
		} else {
			mode_text = "NO";
		}
		printf("0x%u-0x%u %s\n", *(uint32_t *)ptr[i].from, *(uint32_t*)ptr[i].to, mode_text);
	}

	//Change mem
	mmap(0x00000000, 4096, PROT_READ, MAP_SHARED, -1, 0);

	int new_number_of_regions = get_mem_diff(ptr,number_of_regions,diffptr,30);

	printf("Number of regions: %d\n", number_of_regions);
	for (i=0; i < new_number_of_regions; ++i)
	{
		int mode = ptr[i].mode;
		char * mode_text;
		if (mode == 0){
			mode_text = "RW";
		} else if (mode == 1){
			mode_text = "RO";
		} else {
			mode_text = "NO";
		}
		printf("0x%u-0x%u %s\n", *(uint32_t *)ptr[i].from, *(uint32_t*)ptr[i].to, mode_text);
	}
	
	return 0;
}