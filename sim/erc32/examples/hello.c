/* A small test program to demonstrate use of UARTs and clock */


#define RXADATA (int *) 0x01F800E0
#define RXBDATA (int *) 0x01F800E4
#define RXSTAT (int *) 0x01F800E8

int write (fd, buf, nbyte)
int fd;
char *buf;
int nbyte;
{

        int *rxadata;
        int rxmask;
	int nsave;
	volatile *rxstat;

	nsave = nbyte;

	switch (fd) {
		case 0 : 
			rxadata = RXADATA;
			rxmask = 6;
			break;
		case 1 : 
			rxadata = RXBDATA;
			rxmask = 0x60000;
			break;
		default:
			return (-1);
	}
	rxstat = RXSTAT;
        while (nbyte > 0) {
		while ((*rxstat & rxmask) == 0);
		*rxadata = *buf;
		buf++;
		nbyte--;
	}
	return (nsave);
}

int read (fd, buf, nbyte)
int fd;
char *buf;
int nbyte;
{

        int rxmask,*rxadata;
	int nsave;
	volatile *rxstat;

	nsave = nbyte;
	switch (fd) {
		case 0 : 
			rxadata = RXADATA;
			rxmask = 1;
			break;
		case 1 : 
			rxadata = RXBDATA;
			rxmask = 0x10000;
			break;
		default:
			return (-1);
	}
	rxstat = RXSTAT;
        while (nbyte > 0) {
		while ((*rxstat & rxmask) == 0);
		*buf =  *rxadata;
		buf++;
                nbyte--;
	}
	return (nsave);
}

char *
puts(s)
char *s;
{
    while (*s) write(0,s++,1);
}

main()
{
    puts("Hello world!\n");
}

