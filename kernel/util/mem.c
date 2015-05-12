#include <stdint.h>
#include <mem_addr.h>
#include "../mmu.h"
#include "../kernel.h"
#include "mem.h"
#include "log.h"

extern uint8_t end;

void *ksbrk(intptr_t increment) {
	static void * heap_end = &end;
	void *old_heap_end = heap_end;
	uint32_t tmp, new, pages, i;
	
	kprintf(LOG_LEVEL_DEBUG, "MEM: sbrk inc %i\n", increment);
	
	if(increment > 0) {
		tmp = ((((uint32_t) heap_end) + (MMU_PAGE_SIZE - 1)) & ~MMU_PAGE_MASK);
		new = ((((uint32_t) heap_end) + increment + (MMU_PAGE_SIZE - 1)) & ~MMU_PAGE_MASK);
		pages = (new - tmp)/MMU_PAGE_SIZE;
		
		for(i = 0; i < pages; i++) {
			mmu_alloc_at((void *) (tmp + i*MMU_PAGE_SIZE), true, false);
		}			
	} else if(increment < 0) {
		//TODO: implement
	}
	
	heap_end += increment;
	kprintf(LOG_LEVEL_DEBUG, "MEM: sbrk heap is now @ 0x%X\n", heap_end);
	return old_heap_end;
}

void *memset(void *pointer, int c, uint32_t n) {
	uint8_t *p1 = pointer;
	while(n) {
		p1[--n] = c;
	}
	return NULL;
}

void *memcpy(void *dest, void *src, uint32_t n) {
	uint8_t *p1 = dest, *p2 = src;
	while(n) {
		p1[n] = p2[n];
		n--;
	}
	
	return NULL;
}

/**
 * malloc - Implementation of malloc et al
 * malloc.c - Memory allocation library functions
 * @author Axel Isaksson <axelis@kth.se>
 * @copyright MIT/X11 License, see COPYING
 * 
 * Contains the library functions malloc(3), free(3) and realloc(3).
 * Several algorithms can be used for allocation, see README
 * 
 */


/*Macros*/
#define ALIGN(x) ((x + (sizeof(long) - 1)) & ~(sizeof(long) - 1))
#define ALIGNWITH(x, y) ((x + ((y) - 1)) & ~((y) - 1))
#define MIN(x, y) ((x) > (y) ? (y) : (x))

#ifndef STRATEGY
#define STRATEGY 4
#endif

/*This is all the difference between best fit and worst fit*/
#if STRATEGY == 3
#define FIT_COMPARE >
#else
#define FIT_COMPARE <
#endif

#ifndef NRQUICKLISTS
#define NRQUICKLISTS 4
#endif

typedef struct Header Header;
struct Header {
	size_t size;
	Header *next;
};

/*Pointer to the linked lists with free blocks*/
static Header *freelist[NRQUICKLISTS + 1];
/*Keep statistics of total used mem*/
size_t malloc_used_mem = 0;


/**
 * Allocate a new block on the heap from the system.
 * The block will be aligned to a page boundary and the block
 * size will be rounded upwards to the next page.
 * 
 * @param size Size in bytes excluding header
 * @return Newly allocated block of ram, beginning with a header
 */
static Header *getmore(size_t size) {
	Header *header;
	
	size = ALIGNWITH(size + sizeof(Header), MMU_PAGE_SIZE);
	malloc_used_mem += size;
	header = ksbrk(size);
	if(header == (void *) - 1)
		return NULL;
	header->size = size - sizeof(Header);
	
	return header;
}


/**
 * Allocate memory
 * 
 * @param size Size in bytes to be allocated
 * @return Pointer to avaible block of memory, or NULL on failure
 */
void *kmalloc(size_t size) {
	Header **iter, *header;
	#if STRATEGY != 1
	Header **tmp;
	#endif
	#if STRATEGY == 4
	int list, i;
	
	for(list = 0; list < NRQUICKLISTS && size > (8 << list); list++);
	#endif
	
	kprintf(LOG_LEVEL_DEBUG, "MEM: malloc called with %u bytes\n", size);
	
	if(!size)
		return NULL;
	
	for(;;) {
		#if STRATEGY == 1
		/*First fit*/
		for(iter = &freelist[NRQUICKLISTS]; *iter && (*iter)->size < size; iter = &(*iter)->next);
		#elif STRATEGY == 4
		/*Quick fit*/
		for(iter = &freelist[list]; *iter && (*iter)->size < size; iter = &(*iter)->next);
		#else
		/*Best/worst fit*/
		tmp = NULL;
		for(iter = &freelist[NRQUICKLISTS]; *iter; iter = &(*iter)->next)
			if((*iter)->size >= size && (!tmp || (*iter)->size FIT_COMPARE (*tmp)->size))
				tmp = iter;
		if(tmp)
			iter = tmp;
		#endif
		if(*iter) {
			header = *iter;
			if(ALIGN(header->size) > (ALIGN(size) + sizeof(Header))) {
				/*Split the block up*/
				Header *newheader = (Header *)(((char *) (header + 1)) + ALIGN(size));
				newheader->size = ALIGN(header->size) - (ALIGN(size) + sizeof(Header));
				header->size = ALIGN(size);
				newheader->next = header->next;
				*iter = newheader;
			} else
				*iter = header->next;
			
			kprintf(LOG_LEVEL_DEBUG, "MEM: malloc return ptr=0x%X\n", header + 1);
			return header + 1;
		} else {
			/*No free, large enough blocks availble, get more from system*/
			if((header = getmore(size))) {
				#if STRATEGY == 4
				if(list < NRQUICKLISTS) {
					/*Split quicklist-blocks into smaller pieces at once*/
					int sz = header->size + sizeof(Header);
					tmp = &header;
					for(i = 0; i < sz/((8 << list) + sizeof(Header)); i++) {
						(*tmp)->size = (8 << list);
						(*tmp)->next = (Header *) (((char *) *tmp) + (8 << list) + sizeof(Header));
						tmp = &(*tmp)->next;
					}
					*tmp = freelist[list];
					freelist[list] = header;
					continue;
				}
				#endif
				kfree(header + 1);
			} else
				return NULL;
		}
	}
}


/**
 * Change the size of an allocated memory block.
 * Can both grow and shrink. Will allocate a new block,
 * move the data and free the old block.
 * 
 * @param ptr Pointer to a previously allocated block of memory
 * @param size New requested size of the block
 * @return Pointer to the new memory block, or NULL on failure
 */
void *krealloc(void *ptr, size_t size) {
	void *tmp;
	Header *header;
	
	if(!ptr)
		return kmalloc(size);
	
	if(!size) {
		kfree(ptr);
		return NULL;
	}
	/*Should directly split block when shrinking without malloc+copy, but too lazy*/
	tmp = kmalloc(size);
	header = ((Header *) ptr) - 1;
	memcpy(tmp, ptr, MIN(header->size, size));
	kfree(ptr);
	return tmp;
}


/**
 * Free allocated memory
 * 
 * @param ptr Pointer to a previously allocated block of memory
 */
void kfree(void *ptr) {
	Header **iter, *header;
	bool hasmerged = false;
	#if STRATEGY == 4
	int list;
	#endif
	kprintf(LOG_LEVEL_DEBUG, "MEM: free called with ptr=0x%X\n", ptr);
	if(!ptr)
		return;
	
	header = ((Header*) ptr) - 1;
	#if STRATEGY == 4
	for(list = 0; list < NRQUICKLISTS && header->size > (8 << list); list++);
	if(list < NRQUICKLISTS) {
		/*Quicklist blocks are not merged*/
		header->next = freelist[list];
		freelist[list] = header;
		return;
	}
	#endif
	
	redo:
	for(iter = &freelist[NRQUICKLISTS]; *iter; iter = &((*iter)->next)) {
		if(((char *) *iter) + (*iter)->size + sizeof(Header) == (void *) header) {
			/*Merge lower bound*/
			(*iter)->size += sizeof(Header) + header->size;
			header = *iter;
			*iter = (*iter)->next;
			goto redo;
		} else if(((char *) header) + sizeof(Header) + header->size == (void *) *iter) {
			/*Merge upper bound*/
			header->size += sizeof(Header) + (*iter)->size;
			header->next = (*iter)->next;
			*iter = header;
			hasmerged = true;
		}
	}
	if(!hasmerged) {
		header->next = freelist[NRQUICKLISTS];
		freelist[NRQUICKLISTS] = header;
	}
}
