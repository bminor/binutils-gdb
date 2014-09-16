/*
 *	This simple classical example of recursion is useful for
 *	testing stack backtraces and such.
 */

#include <stdio.h>
#include <stdlib.h>

#ifdef PROTOTYPES
int factorial (int);

int
main (int argc, char **argv, char **envp)
#else
int
main (argc, argv, envp)
int argc;
char *argv[], **envp;
#endif
{
#ifdef FAKEARGV
    printf ("%d\n", factorial (1)); /* commands.exp: hw local_var out of scope */
#else    
    if (argc != 2) {
	printf ("usage:  factorial <number>\n");
	return 1;
    } else {
	printf ("%d\n", factorial (atoi (argv[1])));
    }
#endif
    return 0;
}

#ifdef PROTOTYPES
int factorial (int value)
#else
int factorial (value) int value;
#endif
{
    int  local_var;

    if (value > 1) {
	value *= factorial (value - 1);
    }
    local_var = value;
    return (value);
} /* commands.exp: local_var out of scope  */
