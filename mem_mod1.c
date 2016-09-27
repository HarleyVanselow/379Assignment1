#include "memlayout.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char const *argv[])
{
	struct memregion * ptr;
	ptr = (struct memregion*)malloc(sizeof(struct memregion)*30);

	int number_of_regions = get_mem_layout(ptr, 30);
	int i = 0;
	printf("Numer of regions: %d\n", number_of_regions);
	for (i; i < number_of_regions; ++i)
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
		// printf("0x%08X-0x%08X %s\n", *(uint32_t *)ptr[i].from, *(uint32_t*)ptr[i].to, mode_text);
		printf("0x%u-0x%u %s\n", *(uint32_t *)ptr[i].from, *(uint32_t*)ptr[i].to, mode_text);
	}
	
	return 0;
}