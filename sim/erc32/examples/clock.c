
int clock()
{
	volatile int *t_addr;

	t_addr = (volatile int * ) 0x01F80080;	/* Real-time clock address */
	return(*t_addr);
}
