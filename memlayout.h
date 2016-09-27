#ifndef MEMLAYOUT_H
#define MEMLAYOUT_H

#ifndef MEM_RW
#define MEM_RW 0
#endif

#ifndef MEM_RO
#define MEM_RO 1
#endif

#ifndef MEM_NO
#define MEM_NO 2
#endif

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

struct memregion {
void *from;
void *to;
unsigned char mode; /* MEM_RW, or MEM_RO, or MEM_NO */
};

int get_mem_layout (struct memregion *regions, unsigned int size);
int get_mem_diff (struct memregion *regions, unsigned int howmany,
struct memregion *thediff, unsigned int diffsize);

#endif