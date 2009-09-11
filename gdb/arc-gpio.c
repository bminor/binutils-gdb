/* Target dependent code for ARC processor family, for GDB, the GNU debugger.

   Copyright 2005, 2008, 2009 Free Software Foundation, Inc.

   Contributed by ARC International (www.arc.com)

   Authors:
      Richard Stuckey      <richard.stuckey@arc.com>
      A.N. Other           <unknown>

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/******************************************************************************/
/*                                                                            */
/* Outline:                                                                   */
/*     This module implements operations for reading data from / writing data */
/*     to a parallel port using a GPIO (General Purpose Input/Output) driver. */
/*                                                                            */
/******************************************************************************/

/* system header files */
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

/* gdb header files */
#include "defs.h"

/* ARC header files */
#include "arc-gpio.h"


/* -------------------------------------------------------------------------- */
/*                               local data                                   */
/* -------------------------------------------------------------------------- */

// I/O control numbers.

// Linux kernel uses 0x54XX for special purposes.  Avoid such.
// We'll pick large numbers.
// We don't use the linux convention of dividing the IOC into a bunch
// of bit fields.  We can always switch to this later, as the
// IOC 0 returns the driver version (which IOC value will never change).

#define GPIO_IOC_VERSION 0              // returns version
#define GPIO_IOC_BASE    0xaa3a0000UL   // Intended for use on ARCangel 3!


// Switch base address of parallel port.  0x3f8 is assumed.
// WARNING!  You can write on any port whatsoever with this driver.
// BE CAREFUL!
#define GPIO_IOC_SET_PORT_BASE  (GPIO_IOC_BASE + 1) // cmd, arg=port base

// General input/output ioctl.  See GPIO_ioctl struct.
#define GPIO_IOC_DO_IO          (GPIO_IOC_BASE + 2) // cmd, arg=GPIO_ioctl *

// For emergency purposes in case the driver is goofed up.
#define GPIO_IOC_HARD_RESET     (GPIO_IOC_BASE + 3) // cmd, no arg

// Do you have an antiquated parallel port?  You might need to ask
// the driver to use outb_p and inb_p (_p = pause).  Default is not to.
#define GPIO_IOC_SET_PAUSE      (GPIO_IOC_BASE + 4) // arg = 1 => use pause; otherwise not.


/* parallel port I/O addresses (LPT1) */
#define PORT_BASE_ADDRESS     0x378
#define DATA_PORT_ADDRESS     (PORT_BASE_ADDRESS + DATA_PORT)
#define STATUS_PORT_ADDRESS   (PORT_BASE_ADDRESS + STATUS_PORT)
#define CONTROL_PORT_ADDRESS  (PORT_BASE_ADDRESS + CONTROL_PORT)

#define NULL_FILE_DESCRIPTOR    (-1)


typedef struct
{
    // This is used for general input and output in the same ioctl.
    // N.B. "input" is input TO the port, and "output" is output FROM the port.
    //
    // inlen is always even and represents a number of pairs:
    //
    //    [0|1|2, value]              : write value to   port_base+0|1|2.
    //    [0x80|0x81|0x82, <ignored>] : read  value from port_base+0|1|2
    //                                  and append result to outbuf.
    //
    // Thus one can intermix read and write in the same ioctl.

    // inlen is replaced by the number of input bytes consumed.
    unsigned inlen;
    char    *inbuf;

    // outlen is replaced by the number of output bytes written.
    unsigned outlen;
    char    *outbuf;
} GPIO_ioctl;


/* Buffers to hold data read from / written to ports; we generally read/write
   only 1 byte of data at a time, so the buffers need hold only 1 byte pair each.  */
static char       input_buffer [2];
static char       output_buffer[2];
static GPIO_ioctl ioctl_data  = { 0, input_buffer, 0, output_buffer };

/* A file descriptor for the GPIO driver.  */
static int        gpio_driver = NULL_FILE_DESCRIPTOR;

/* Debug data.  */
// static const char name[] = {'D', 'S', 'C'};


/* -------------------------------------------------------------------------- */
/*                               externally visible data                      */
/* -------------------------------------------------------------------------- */

/* This is set to TRUE if an I/O error occurs in accessing the port.  */
Boolean gpio_port_error;


/* -------------------------------------------------------------------------- */
/*                               externally visible functions                 */
/* -------------------------------------------------------------------------- */

/* Initialization of the GPIO interface.  */

Boolean
gpio_open (void)
{
    /* Open the driver, if not already open.  */
    if (gpio_driver == NULL_FILE_DESCRIPTOR)
    {
        gpio_driver = open(GPIO_DEVICE, O_RDWR);

        if ((gpio_driver == NULL_FILE_DESCRIPTOR)   ||
            ioctl(gpio_driver, GPIO_IOC_HARD_RESET) ||
            ioctl(gpio_driver, GPIO_IOC_SET_PORT_BASE, PORT_BASE_ADDRESS))
        {
            warning(_("unable to open JTAG port (device " GPIO_DEVICE "): %s"),
                    strerror(errno));
            return FALSE;
        }

        gpio_port_error = FALSE;
    }

    return TRUE;
}


/* Close the GPIO interface.  */

void
gpio_close (void)
{
    /* Close the driver, if not already closed.  */
    if (gpio_driver != NULL_FILE_DESCRIPTOR)
    {
        /* Close file descriptor opened for communication with gpio driver.  */
        if (close(gpio_driver) == -1)
            warning(_("unable to close JTAG port (device " GPIO_DEVICE "): %s"),
                    strerror(errno));

        gpio_driver = NULL_FILE_DESCRIPTOR;
    }
}


/* Write a byte of data to the given port.  */

void
gpio_write (ParallelPort port, Byte data)
{
    ioctl_data.inlen    = 2;
    ioctl_data.inbuf[0] = (char) port;
    ioctl_data.inbuf[1] = (char) data;

    if (ioctl(gpio_driver, GPIO_IOC_DO_IO, &ioctl_data))
        error(_("Failure writing to port %d: %s"), port, strerror(errno));

    /* If no data has been consumed by the port.  */
    if (ioctl_data.inlen == 0)
        gpio_port_error = TRUE;

//  DEBUG("ioctl_data.inlen: %d\n", ioctl_data.inlen);
//  DEBUG("GPIO write: %02x -> %c\n", data, name[port]);
}


/* Read a byte of data from the given port.  */

Byte
gpio_read (ParallelPort port)
{
    ioctl_data.inlen    = 2;
    ioctl_data.inbuf[0] = (char) (port + 0x80);
//  ioctl_data.inbuf[1] is ignored

    /* N.B. outlen must be set!  */
    ioctl_data.outlen    = 1;
    ioctl_data.outbuf[0] = (char) 0; // in case the read fails

    if (ioctl(gpio_driver, GPIO_IOC_DO_IO, &ioctl_data))
        error(_("Failure reading from port %d: %s"), port, strerror(errno));

    /* If no data has been provided by the port  */
    if (ioctl_data.outlen == 0)
        gpio_port_error = TRUE;

//  DEBUG("ioctl_data.outlen: %d\n", ioctl_data.outlen);
//  DEBUG("GPIO read : %02x <- %c\n", (Byte) ioctl_data.outbuf[0], name[port]);

    return (Byte) ioctl_data.outbuf[0];
}


/* Write a series of bytes of data to the ports.  */

void
gpio_write_array (GPIO_Pair array[], unsigned int num_elements)
{
    char         buffer[num_elements * 2];
    GPIO_ioctl   data = { 0, buffer, 0, NULL };
    unsigned int i;

    for (i = 0; i < num_elements; i++)
    {
        buffer[data.inlen++] = (char) array[i].port;
        buffer[data.inlen++] = (char) array[i].data;
    }

    if (ioctl(gpio_driver, GPIO_IOC_DO_IO, &data))
        error(_("Failure writing to port: %s"), strerror(errno));

    /* If no data has been consumed by the port.  */
    if (ioctl_data.inlen == 0)
        gpio_port_error = TRUE;
}

/******************************************************************************/
