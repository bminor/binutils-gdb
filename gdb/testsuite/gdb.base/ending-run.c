/* Test program for <next-at-end> and
 * <leaves-core-file-on-quit> bugs.
 */
#include <stdio.h>

int callee( x )
int x;
{
    int y = x * x;
    return (y - 2);
}

main()
{

    int *p;
    int i;

    p = (int *) malloc( 4 );
    
    for (i = 1; i < 10; i++)
        {
            printf( "%d ", callee( i ));
            
        }
    printf( " Goodbye!\n" );
}
