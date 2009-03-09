struct GPIO_ioctl {
    // This is used for general input and output in the same ioctl.
    // inlen is replaced by the number of input bytes consumed.
    // inlen is always even and represents a number of pairs:
    //	[0/1/2,value]: write value to port_base+0/1/2.
    //	[0x80/0x81/0x82, --]: read value from port_base+0/1/2
    //	and append result to outbuf.
    // Thus one can intermix read and write in the same ioctl.
    unsigned inlen; char *inbuf;
    // outlen is replaced by # of output bytes written.
    unsigned outlen; char *outbuf;
    };

// IO control numbers

// Linux kernel uses 0x54XX for special purposes.  Avoid such.
// We'll pick large numbers.
// We don't use the linux convention of dividing the IOC into a bunch
// of bit fields.  We can always switch to this later, as the
// IOC 0 returns the driver version (which IOC value will never change).

#define GPIO_IOC_VERSION 0	// returns version
#define GPIO_IOC_BASE 0xaa3a0000	// Intended for use on ARCangel 3!

// Switch base address of parallel port.  0x3f8 is assumed.
// WARNING!  You can write on any port whatsoever with this driver.
// BE CAREFUL!
#define GPIO_IOC_SET_PORT_BASE 	(GPIO_IOC_BASE+1) // cmd, arg=port base

// General input/output ioctl.  See GPIO_ioctl struct.
#define GPIO_IOC_DO_IO  	(GPIO_IOC_BASE+2) // cmd, arg=GPIO_ioctl *

// For emergency purposes in case the driver is goofed up.
#define GPIO_IOC_HARDRESET 	(GPIO_IOC_BASE+3) // cmd, no arg

// Do you have an antiquated parallel port?  You might need to ask
// the driver to use outb_p and inb_p (_p = pause).  Default is not to.
#define GPIO_IOC_SET_PAUSE 	(GPIO_IOC_BASE+4) // arg = 1 => use pause; o/wise not.
