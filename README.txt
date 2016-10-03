Kdan Hillier 1393746
Harley Vanselow 1388813

Compile: make all
Run: ./mem_modX.out where X is 1, 2, or 3

Questions:

In mem_mod1, a large malloc() call is made after the first memory mapping, which results in an increase of the process's heap size. 
This manifests as the growth of a read-write region in memory into increasingly higher memory addresses, which represents the heap growing up.
Malloc calls result in heap growth rather than stack growth because it does not represent a variable instantiation.  
Rather, it sets aside free memory space for use by the process, accessible by pointer.

In mem_mod2, a large char array is instantiated, which results in an increase to the process's stack size. 
This manifests as the growth of a read-write region in memory into increasing smaller memory address, which represents the stack growing down. 
Since the array is a local variable, it uses memory from the stack. 
In order for this instantiation to have the desired effect, it, and the call to get_mem_diff had to be made in a method separate from main. 
This is because the compiler will otherwise instantiate the large array at the start of main, no matter the line the instantiation was actually 
made on. As a result, the stack growth is picked up on in the first memory map, and no difference is reported. 
By moving the instantiation to a separate method, it can be guaranteed the stack is unaffected by the instantiation until after the first
memory map is made.

In mem_mod3, a call to mmap is made before the first memory map is performed, which requests a certain number of pages have their accessibility
permissions changed to read only. Since a NULL is passed in to mmap's address parameter, the system decides where to apply this mapping, and a 
pointer to the start of the mapping is returned. After the memory is mapped, another call to mmap is made, this time using the address the 
previous mmap returned to target the memory region. This time, the accessibility is set to read-write, and the MAP_FIXED flag is passed, which 
forces mmap to apply the change at exactly the address passed in. Since the second mmap is told to apply its new mapping to the same amount of 
memory as in the first call, only the previously generated region is changed, and other regions are unaffected.


The Weird Thing:
Certain extra regions were changed in the memory without any input from our program, separate from the regions we purposefully changed. 
This is probably the operating system doing things that we have no control over.


Memory Region Identification:
0x00000000 to 0x08048000 is no access, indicating the kernel text and data storage section

The regions between 0x0804000 and 0x40000000 hold, in order, the ELF, .data, and .bss data

0x40000000-0x40020FFF holds the shared data and libraries

The read-write region staring at 0x401E4000 represents the heap for the process

The no-access region immediately after the heap is the free space allocated to allow the
stack to grow down into it, and the heap to grow up into it.

The read-write region immediately before the last region is the stack

The no access region ending at 0xFFFFFFFF is the kernel virtual memory block


