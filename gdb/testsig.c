# include	<signal.h>

main ()
{
	int	handle ();
	int	i;
	signal (SIGALRM, handle);
	alarm (5);
	for (i = 0; i < 100000; i++)
		printf ("%d\n", i);
}

handle ()
{
	printf ("signal!\n");
	alarm (5);
}
