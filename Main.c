#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "MemPool.h"

int main ( int argc, char * argv[] )
{
    int       i;
	MemPool * pool;
    uchar   * h, * j;
    pool = mpCreatePool ( 3 * 1024 * 1024 );
    for ( i = 0; i < 100; i++)
    {
        h = mpAlloc ( pool, sizeof ( " Hello, Xsoda at Emacs! " ) );
        strcpy ( h, " Hello, Xsoda at Emacs! ");
        printf ( "%d.%s --- 0x%08X\n", i, h, h );
        mpFree ( pool, h );
        
        j = mpAlloc ( pool, sizeof ( " Hello, Xsoda! " ) );
        strcpy ( j, " Hello, Xsoda! " );
        printf ( "%d.%s --- 0x%08X\n", i, j, j );
    }

    mpDestoryPool ( pool );
    system ( "pause" );
    return 0;
}
