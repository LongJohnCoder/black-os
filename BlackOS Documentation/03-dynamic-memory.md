# Dynamic Memory

Dynamic memory is used when an execution program asks the operating system to give it a block of the main memory. This allocation happends at runtime, which means the the kernel does not know about the allocation until it happends. There are many different implementations of dynamic memory, each has some advantages and some disadvantages. The allocation algorithm used in the BlackOS is based upon explicit free lists. 

## Overview

The dynamic memory algorithm is based upon continous tracking of free memory blocks. A memory block is a pysical unit of memory. Every memory block starts with 8 bytes of information. This is called a memory descriptor and contains the block size in number of bytes, and a pointer to another block. In order to speed up the allocation time, only free blocks are tracked. One of the features required by the kernel, is the ability to allocate memory in a spesific section. This is due to DRAM access rates being slow when accessing different banks. 

In order to use the dynamic memory it must be configured first. The includes initialization of all the different memory sections (DRAM BANK0, DRAM BANK1, SRAM, osv.). The memory should be reset to zero, and the descriptors should be updated. More info later. After the configuration each memory section consists of one big block of free memory 
