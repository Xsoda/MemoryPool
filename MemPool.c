#include <stdlib.h>
#include <string.h>
#include "MemPool.h"

MemPool * mpCreatePool ( uint size )
{
    MemPool * p;
    size = size < sizeof ( MemPool ) ? DEFAULT_SIZE : algin ( size );
    p = ( MemPool * ) malloc ( size );
    if ( p != NULL )
        memset ( p, 0, size );

    p -> data.last  = ( uchar * ) p + sizeof ( MemPool );
    p -> data.end   = ( uchar * ) p + size;
    p -> data.max   = size - sizeof (MemPool );
    p -> poolstate  = EMPTY;

    return p;
}
void mpDestoryPool ( MemPool * pool )
{
    /* free the large space */
    MemPoolLarge   * l;
    MemPool        * p;
	if ( pool == NULL )
		return;
    for ( l = pool -> large; l; l = l -> next )
        if ( l -> buf != NULL )
            free ( l -> buf );

    /* free the pool spzce */
    do
    {
        p = pool -> next;
        free ( pool );
        pool = p;
    }while ( pool );
}

uchar * mpAllocFromJunk ( MemPool * pool, uint size )
{
    MemPoolChain  * c;          /* alloc space from already freed space */
    MemPool       * p;
	
    for ( p = pool; p; p = p -> next )
        if ( p -> free != 0 )
        {
            for ( c = p -> chain; c; c = c -> next )
                if ( c -> used == FREE && c -> size >= size )
                {
                    c -> used = USING;
                    p -> free --;
                    return c -> buf;
                }
        }
    return NULL;
}

uchar * mpAllocLarge ( MemPool * pool, uint size )
{
    MemPoolLarge  * l;
    MemPool       * p;

    for ( p = pool; p; p = pool -> next)
	{
        for ( l = p -> large; l; l = l -> next )
		{
			if ( l -> buf == NULL )
				l -> buf = ( uchar * ) malloc ( size );
			if ( l -> buf != NULL )
				return l -> buf;
		}
		if ( p -> data.end - p -> data.last >= sizeof ( MemPoolLarge ) )
        {
            l = ( MemPoolLarge * ) p -> data.last;
			p -> data.last += sizeof ( MemPoolLarge );
            l -> buf = ( uchar * ) malloc ( size );
            if ( l -> buf == NULL )
                return NULL;
            l -> next = p -> large;
            p -> large = l;
            return l -> buf;
        }
        else
            p -> poolstate = FULL;
	}
    p = mpCreatePool ( pool -> data.max + sizeof ( MemPool ) ); /* alloc fail? maybe current pool */
    p -> next = pool -> next;                                   /* cannot storege a MemPoolLarge */
    pool -> next = p;                                           /* struct... we creat a new one, */
    return mpAllocLarge ( pool, size );                         /* recursion call itself. */
}

uchar * mpAlloc ( MemPool * pool, uint size )
{
    uchar    * m;
    MemPool  * p;
    MemPoolChain * c;
    size = algin ( size );
	
	if ( pool == NULL )
		return;
    if ( size <= pool -> data.max ) /* need space is too large, direct alloc use malloc() */
    {
        p = pool;
        m = mpAllocFromJunk ( p, size );
        if ( m != NULL )
            return m;
        do
        {
            /* total space is real space size add store alloc info struct */
            if ( (uint)(p -> data.end) - (uint)(p -> data.last) >= size + sizeof ( MemPoolChain ) )
            {
				c = ( MemPoolChain * ) ( p -> data.last );
				p -> data.last += sizeof ( MemPoolChain );
				c -> next = p -> chain;
				p -> chain = c;
                
				c -> size = size;
				m = p -> data.last;
				c -> buf = m;
				c -> used = USING;
                p -> data.last += size;
                p -> alloc ++;
                return m;
            }
            
            p = p -> next;
        }while ( p );

        
        p = mpCreatePool ( pool -> data.max + sizeof ( MemPool ) ); /* alloc fail? maybe pool is full, creat */
        p -> next = pool -> next;                                   /* a new pool, go on. */
        pool -> next = p;
        return mpAlloc ( pool, size );
    }

    return mpAllocLarge ( pool, size );
}

void mpFree ( MemPool * pool, uchar * p )
{
    MemPoolLarge  * l;
    MemPoolChain  * c;

    for ( ; pool; pool = pool -> next )
    {
        for ( l = pool -> large; l; l = l -> next ) /* we start free the space from large list first. */
            if ( p == l -> buf )
			{
                free ( l -> buf );
				l -> buf = NULL;
				return;
			}

        for ( c = pool -> chain; c; c = c -> next ) /* if we cannot free it in large list, test free in small space list. */
            if ( p == c -> buf )
            {
                c -> used = FREE;
                pool -> free ++;
				return;
            }
    }
}
void mpReset ( MemPool * pool )
{
    MemPool      * p;
    MemPoolLarge * l;
    for ( p = pool; p; p = p -> next )
    {
        for ( l = p -> large; l; l = l -> next )
            if ( l -> buf != NULL )
                free ( l -> buf );
        p -> chain = NULL;
        p -> alloc = p -> free = 0;
        p -> large = NULL;
        p -> data.last = ( uchar * ) p + sizeof ( MemPool );
    }
}
   
