#ifndef _MEMPOOL_H_
#define _MEMPOOL_H_

#define uchar unsigned char
#define uint  unsigned int
#define ulong unsigned long

#define USING   0
#define FREE    1
#define FULL    1
#define EMPTY   0

#define algin(x) ( ( x + 3 ) >> 2 ) << 2 /* in this code, all size will algined by 4 byte */

#define DEFAULT_SIZE 3 * 1024 * 1024 /* 3 MB, it's default memory pool's size */

typedef struct tagMemPoolChain{ /* samll space link */
    uchar   				* buf;  /* pointer the buffer, it's useable memory */
    uint      		  		  size; /* the buf's size */
    uint       		  		  used; /* sign buf whether useable */
    struct tagMemPoolChain  * next;
} MemPoolChain;

typedef struct tagMemPoolData{  /* the infomation of pool, it very inportant in alloc space */
    uint          			  max; /* current pool's useable size */
    uchar         			* last; /* pointer of current pool recent useable position */
    uchar         			* end;  /* pointer of current pool's end */
} MemPoolData;

typedef struct tagMemPoolLarge{ /* large space is alloced by malloc() function direct, */
    uchar         			* buf; /* so there is't size attribute in it. */
    struct tagMemPoolLarge  * next;
} MemPoolLarge;

typedef struct tagMemPool{      /* memory pool's link list, it will link many pools */
    MemPoolData      		  data;  /* current pool's info */
    MemPoolChain  			* chain; /* the head pointer of small space */
    MemPoolLarge  			* large; /* the head pointer of large space */
    struct tagMemPool       * next;  /* point the next pool */
    uint            		  poolstate; /* current pool's state, it signs this pool is full or empty */
    uint            		  alloc; /* the small space's alloc time */
    uint            		  free; /* the small space's free time */
} MemPool;

MemPool * mpCreatePool ( uint size ); /* create pool, and initialize, prealloc space */
uchar   * mpAlloc      ( MemPool * pool, uint size ); /* alloc from pool */
void      mpReset      ( MemPool * pool );            /* reset the pool info, it will free large space and small space */
void      mpFree       ( MemPool * pool, uchar * p ); /* free alloced pool space and large space */
void      mpDestoryPool( MemPool * pool );            /* destory the memory pool, call free() function free all space */

#endif /* _MEMPOOL_H_ */
