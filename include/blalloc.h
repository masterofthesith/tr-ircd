/*
 * blalloc.h - Based roughly on Wohali's old block allocator.
 *
 *   Copyright (C) 2000-2003 TR-IRCD Development
 *
 * Mangled by Aaron Sethman <androsyn@ratbox.org>
 * Below is the original header found on this file
 *
 * File:   blalloc.h
 * Owner:   Wohali (Joan Touzet)
 *
 *
 * $Id: blalloc.h,v 1.4 2003/07/01 11:01:18 tr-ircd Exp $
 */

#ifndef INCLUDED_blalloc_h
#define INCLUDED_blalloc_h

#ifndef NOBALLOC

#include "tools.h"

#define BlockHeapAlloc(x) _BlockHeapAlloc(x)
#define BlockHeapFree(x, y) _BlockHeapFree(x, y)

/* 
 * Block contains status information for an allocated block in our
 * heap.
 */


struct Block {
	int		freeElems;		/* Number of available elems */
	size_t		alloc_size;
	struct Block*	next;			/* Next in our chain of blocks */
	void*		elems;			/* Points to allocated memory */
	dlink_list	free_list;
	dlink_list	used_list;					
};

typedef struct Block Block;

struct MemBlock {
	dlink_node self;		
	Block *block;				/* Which block we belong to */
};
typedef struct MemBlock MemBlock;

/* 
 * BlockHeap contains the information for the root node of the
 * memory heap.
 */
struct BlockHeap {
   size_t  elemSize;                    /* Size of each element to be stored */
   int     elemsPerBlock;               /* Number of elements per block */
   int     blocksAllocated;             /* Number of blocks allocated */
   int     freeElems;                   /* Number of free elements */
   Block*  base;                        /* Pointer to first block */
};

typedef struct BlockHeap BlockHeap;


extern BlockHeap* BlockHeapCreate(size_t elemsize, int elemsperblock);
extern int BlockHeapDestroy(BlockHeap *bh);
extern int BlockHeapGarbageCollect(BlockHeap *);
extern void initBlockHeap(void);
extern int _BlockHeapFree(BlockHeap *bh, void *ptr);
extern void *_BlockHeapAlloc(BlockHeap *bh);

#else /* NOBALLOC */

//typedef struct BlockHeap BlockHeap;

#define BlockHeap void

#define initBlockHeap()
#define BlockHeapGarbageCollect(x)
#define BlockHeapCreate(es, epb) ((BlockHeap*)(es))
#define BlockHeapDestroy(x)
#define BlockHeapAlloc(x) MyMalloc((int)x)
#define BlockHeapFree(x,y) MyFree(y)
#endif /* NOBALLOC */

extern BlockHeap *free_local_aClients;
extern BlockHeap *free_remote_aClients;
extern BlockHeap *free_channels;
extern BlockHeap *chan_members;
extern BlockHeap *hashent_freelist;
extern BlockHeap *throttle_freelist;
extern BlockHeap *dnode_heap;
extern BlockHeap *maskitem_entries;

#endif /* INCLUDED_blalloc_h */

