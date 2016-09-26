#include <setjmp.h>

#define MEM_RW 0
#define MEM_RO 1
#define MEM_NO 2
#define PAGE_SIZE 4096

uint32_t current_memory_pointer=0;
sigjmp_buf env;

struct memregion {
	void *from;
	void *to;
	unsigned char mode; /* MEM_RW, or MEM_RO, or MEM_NO */
};
void handle_seg_fault(int sig)
{
	printf("Encountered seg fault at %d",current_memory_pointer);
	current_memory_pointer+=PAGE_SIZE;
	siglongjmp(env,1);
}

int get_mem_layout (struct memregion *regions, unsigned int size)
{
	(void) signal(SIGSEGV,handle_seg_fault);
	do{
		sigsetjmp(env,1);
		char* data = (char*)current_memory_pointer;
		char dereferenced_data = &data;
		current_memory_pointer+=PAGE_SIZE;
	}while(current_memory_pointer>0)
	
};
int get_mem_diff (struct memregion *regions, unsigned int howmany,
struct memregion *thediff, unsigned int diffsize);
