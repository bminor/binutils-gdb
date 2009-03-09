/* Target dependent code for ARC700, for GDB, the GNU debugger.

   Copyright 2005 Free Software Foundation, Inc.

   Contributed by Codito Technologies Pvt. Ltd. (www.codito.com)

   Authors: 
      Sameer Dhavale <sameer.dhavale@codito.com>
      Soam Vasani <soam.vasani@codito.com>

   This file is part of GDB.
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include "arc-jtag-ops.h"
#include "gpio.h"
#include <signal.h>
#include <defs.h>
#include <sys/ioctl.h>
#include "arc-tdep.h"
#define printf printf_filtered


unsigned int arcjtag_retry_count = 50;


/* ------------------------------------ */
/*         For ARC jtag Cable           */
/*                                      */
/* Pin no.  Signal           Word , Bit */
/*                                      */
/* - 	    TRST 	                */
/* 8 	    TMS 	     Data 6     */
/* 1 	    TCK 	     Control 0  */
/* 9 	    TDI 	     Data 7     */
/* 13 	    TDO 	     Status 4   */


#define JTAG_TRST 0x00  /* not there */
#define JTAG_TMS  0x40  /* data port */
#define JTAG_TCK  0x01  /* control port. Driven Low. */

#define JTAG_TDI  0x80  /* data port */
#define JTAG_TDO  0x10  /* status port */

/* ------------------------------------ */

/* */
#define A4_HALT_VALUE 0x02000000
#define ARC700_HALT_VALUE 0x1


/* Parallel port i/o addr. (LPT1) */
#define DATA_PORT    0x378
#define STATUS_PORT  0x379
#define CONTROL_PORT 0x37A


unsigned char tapc_dataport=0, tapc_statusport=0, tapc_controlport=0x1;
int fd;
struct GPIO_ioctl jtag_ioctl;
sigset_t block_mask;

enum AA3SSPinState
  {
    READ_FROM_AA3 = 0,
    WRITE_TO_AA3 = 1
  };
enum AA3SSPinState rw_flag; 



struct jtag_ops arc_jtag_ops;
static enum ARCProcessorVersion ARCProcessor = UNSUPPORTED;


/* Sanity check to give error if jtag is not opened at all. */

static void 
check_and_raise_error_if_jtag_closed (void)
{
  if( arc_jtag_ops.jtag_status == JTAG_CLOSED )
    error ("JTAG connection is closed. Use target arcjtag first\n");
}


/* Initializations for GPIO interface */
static int
gpio_setup (void)
{
  fd=open("//dev//gpio",O_RDWR);
  ioctl(fd,GPIO_IOC_HARDRESET);
  ioctl(fd,GPIO_IOC_SET_PORT_BASE,0x378);
  jtag_ioctl.inlen=0;
  jtag_ioctl.outlen=0;
  jtag_ioctl.inbuf=(unsigned char *)xmalloc(2*sizeof(unsigned char));
  jtag_ioctl.outbuf=(unsigned char *)xmalloc(2*sizeof(unsigned char));
  return 0;
}

void gpio_write(unsigned int port, unsigned char *data)
{
    jtag_ioctl.inlen=2;
    jtag_ioctl.inbuf[1]=*data;
    switch(port)
    {
    case DATA_PORT:
	jtag_ioctl.inbuf[0]=0;
	break;

    case STATUS_PORT:
	jtag_ioctl.inbuf[0]=1;
	break;

    case CONTROL_PORT:
	jtag_ioctl.inbuf[0]=2;
	break;
	    
    default:
	error("Invalid port\n");
    }

 
    if(ioctl(fd,GPIO_IOC_DO_IO,&jtag_ioctl))
	error("Failure writing to port 0x%x\n",port);

}


unsigned char gpio_read(unsigned int port)
{
    //jtag_ioctl.inbuf[1]=tapc_statusport;
    jtag_ioctl.inlen=2;
    jtag_ioctl.outlen=1;

    switch(port)
    {
    case DATA_PORT:
	jtag_ioctl.inbuf[0]=0x80;
	break;

    case STATUS_PORT:
	jtag_ioctl.inbuf[0]=0x81;
	break;

    case CONTROL_PORT:
	jtag_ioctl.inbuf[0]=0x82;
	break;
	    
    default:
	error("Invalid port\n");
    }
    
    if(ioctl(fd,GPIO_IOC_DO_IO,&jtag_ioctl))
	error("Failure reading from port 0x%x\n",port);
	  
    return jtag_ioctl.outbuf[0];


}

/* Helper functions for setting
   TMS / TCK / TDI. Valid inputs
   are 1 and 0. The tapc_<tms/tck/tdi> 
   functions set the internal values 
   to be written out to the port. The 
   final values are written only by doing 
   the pulse.
 */

static void
tapc_set_tms (char x)
{
    if(x)
	tapc_dataport |= JTAG_TMS;
    else
	tapc_dataport &= ~JTAG_TMS;

    /* outb(tapc_dataport, DATA_PORT); */
    gpio_write(DATA_PORT,&tapc_dataport);
}


/* Set TCK. */
static void
tapc_set_tck (char x)
{
  /* active low. The clock is active low. */
    if(!x) 
  	tapc_controlport |= JTAG_TCK;
    else
	tapc_controlport &= ~JTAG_TCK;
    
    if(rw_flag == READ_FROM_AA3)
      tapc_controlport |= 0x4; 
    else
      tapc_controlport &= ~(0x4); 
      
    /* outb(tapc_controlport, CONTROL_PORT); */
    gpio_write(CONTROL_PORT,&tapc_controlport);
}


static void
tapc_set_tdi (char x)
{
    if(x)
	tapc_dataport |= JTAG_TDI;
    else
	tapc_dataport &= ~JTAG_TDI;
    
    /* outb(tapc_dataport, DATA_PORT); */
    gpio_write(DATA_PORT,&tapc_dataport);
}

/* Unused function clockdelay. Why not add a command
 that allows the user to set the clock delay ? */

static void
clockdelay (void)
{
    int i;
    //for (i=0; i<10; ++i)
    
    //   usleep(0);
}

/* Clock the JTAG on the ARC platform. */
static void
tapc_pulse (void)
{
    /* Control 0 bit is active low */
    unsigned char temp;
    assert( (tapc_controlport & JTAG_TCK) );  // clock should be zero on entry
   
    /* outb(tapc_controlport & ~JTAG_TCK, CONTROL_PORT); */
    temp = tapc_controlport & ~JTAG_TCK;
    gpio_write(CONTROL_PORT,&temp);
   /*  outb(tapc_controlport, CONTROL_PORT); */
    gpio_write(CONTROL_PORT,&tapc_controlport);
    
}

/* All the JTAG state machine handlers.  */

/* Reset the TAPC controller on the JTAG. 
 */
static void
tapc_reset (void)
{
    /* from any state, these many ones should get us into "test-logic reset" 
     */
    tapc_set_tms(1);
    tapc_set_tck(0);  /* want rising edge */
    tapc_pulse();
    tapc_pulse();
    tapc_pulse();
    tapc_pulse();
    tapc_pulse();
    tapc_pulse();
}

/* Set the tms to the value of the bit and 
   clock the jtag. */

static void
tapc_tms (char x)
{
    tapc_set_tms(x);
    tapc_pulse();
}

/* Read bit from the TDO of the JTAG. */
static char
tapc_readbit (void)
{
    if(gpio_read(STATUS_PORT) & JTAG_TDO)
	return 1; 
    else
	return 0;
}


/* Interface functions that use the below mentioned 
   JTAG state machine handler functions. 
*/

/* Shift one bit out on the JTAG TDI. */
static char
tapc_shiftbit (char x)
{
    char read;

    //printf("tapc_shiftbit: Shifted %d\n", x);

    read = tapc_readbit();    
    tapc_set_tdi(x);
    tapc_pulse();

    return read;
}


/*
 * Shift N bits from to_write into TDI, and out from TDO into read.
 *
 * If msb_first=0, shift LSB first, starting from to_write[0], to_write[1],
 * etc.
 
 * If msb_first=1, shift to_write[-1] MSB first, then to_write[-2] etc.
 *
 * Must be called in Shift DR/IR state.
 * Leaves in Exit1 DR/IR state.
 */ 
static void
tapc_shiftnbits (int n, 
		unsigned char *to_write, 
		unsigned char *read, 
		char msb_first)
{
    unsigned char outbyte, inbyte;
    int nbytes = (n-1)/8 + 1,limit=8;
    int i, j;

    for(i=0; i < nbytes; ++i)
    {
	if(msb_first)
	    outbyte = to_write[-1-i];
	else	   
	    outbyte = to_write[i];

	inbyte = 0;
	/* should write a maximum of 8 bits */
	if(i == nbytes-1)
	    limit = ((n-1) % 8) + 1;  
	
	for(j = 0; j < limit; ++j)
	{
	    unsigned char outbit, inbit;

	    if(msb_first)
		outbit = !!(outbyte & 0x80);
	    else
		outbit = outbyte & 1;
	    /* the last bit of the last byte */
	    /* transition to EXIT-1 state before last bit */
	    if((i == nbytes-1)&&(j == limit-1)) 
	      tapc_set_tms(1); 
	    
	    inbit = tapc_shiftbit(outbit);

	    if(msb_first)
	      {
		inbyte |= (inbit << (7-j));
		outbyte <<= 1;
	      }
	    else
	      {
		inbyte |= inbit << j;
		outbyte >>= 1;
	    }
	}

	//tapc_tms(1);
	if(msb_first)
	    read[-1-i] = inbyte;
	else
	    read[i] = inbyte;
    }
}


/* Read the JTAG status register. This indicates
   the status of the JTAG for the user. 
*/
static unsigned int
read_jtag_status_reg (void)
{
    unsigned int wr, rd;
    int x;
    //rw_flag=0;
    //tapc_tms(0); // runtest/idle
    tapc_tms(1); // select dr
    tapc_tms(1); // select ir
    tapc_tms(0); // capture ir
    tapc_tms(0); // shift ir

    wr = 0x8;    // IR = status register

    tapc_shiftnbits(4, (unsigned char *)&wr, (unsigned char*)&rd, 0);

    
    
    // goto shift DR
    tapc_tms(1); // update ir
    //tapc_tms(0); // runtest/idle
    tapc_tms(1); // select dr
    tapc_tms(0); // capture dr
    tapc_tms(0); // shift dr

    rd = 0;

    // read 1 bit, if it is zero then keep reading
    rd = tapc_shiftbit(0);
    if (rd)
	return rd; 

    rd |= tapc_shiftbit(0) << 1;
    if (rd)
	return rd;
    
    rd |= tapc_shiftbit(0) << 2;

    /* the last bit is optional */
    /*rd |= tapc_shiftbit(0) << 3;*/
    
    return rd;    
}


/* Interpret the status message. */
static void
print_jtag_status_reg_val (unsigned int status)
{
    int i ;
    char * messages [] = { "Stalled" , "Failure", "Ready", "PC Selected" };
    for(i=0;i<=3;i++)
    {
	printf_filtered("%s %s \t",(status & 1)?"":"Not",messages[i]);
	status = status >> 1;
    }
    printf_filtered("\n");
}



/* Write a JTAG Command to a JTAG register. 
   enter in update dr/ir state
   or Test-Logic-Reset.
   exit in update dr
*/
static void
write_jtag_reg (char regnum, unsigned int data, int ndatabits)
{
    unsigned int wr=0,rd=0;
    rw_flag = WRITE_TO_AA3 ;
    //    tapc_tms(0); // runtest/idle
    tapc_tms(1); // select dr
    tapc_tms(1); // select ir
    tapc_tms(0); // capture ir
    tapc_tms(0); // shift ir

		
    tapc_shiftnbits(4, (unsigned char *)&regnum, (unsigned char *)&rd, 0);

    
    tapc_tms(1); // update ir

    tapc_tms(1); // select dr
    tapc_tms(0); // capture dr
    tapc_tms(0); // shift dr

    tapc_shiftnbits(ndatabits, (unsigned char *)&data, 
		    (unsigned char *)&rd, 0);
    tapc_tms(1); // update dr
}
	    

// enter in update dr/ir state
// exit in update dr
static unsigned int
read_jtag_reg (char regnum, int ndatabits)
{
    unsigned int wr=0x0,rd=0;
    rw_flag = READ_FROM_AA3;
    //    tapc_tms(0); // runtest/idle
    tapc_tms(1); // select dr
    tapc_tms(1); // select ir
    tapc_tms(0); // capture ir
    tapc_tms(0); // shift ir

    tapc_shiftnbits(4, (unsigned char *)&regnum, (unsigned char *)&rd, 0);
    tapc_tms(1); // update ir

    /* JTAG registers can be read without going to run-test/idle state.
       
       Doing tapc_tms(0) will take us to run-test/idle state.
       This will make JTAG perform the transaction as per TCR.
       We dont want this. 
    */
    //    tapc_tms(0); // runtest/idle
    tapc_tms(1); // select dr
    tapc_tms(0); // capture dr
    tapc_tms(0); // shift dr

    tapc_shiftnbits(ndatabits, (unsigned char *)&wr, (unsigned char *)&rd, 0);
    tapc_tms(1); // update dr

    return rd;
}



    
    



static int
arc_jtag_read_core_reg (unsigned int regnum, unsigned int *readbuf)
{
    unsigned int rd, wr, data, i;
    check_and_raise_error_if_jtag_closed();
    tapc_reset();
    tapc_tms(0);//run-test idle
    write_jtag_reg(0xA, regnum, 32);//update dr

    

    // Setup instruction register to 0x9 indicating
    // a JTAG instruction is being downloaded.
    // jtag transaction command reg = 0x5 (read core reg)
    write_jtag_reg(0x9, 0x5, 4);//update dr

    /* Perform the transaction.
     */
    tapc_tms(0); // run-test idle
    
    // poll the status

    for (i=0;i<arcjtag_retry_count;i++)
    {
	unsigned int status = read_jtag_status_reg();
	//if( !(status & 1) && (status & 4) )
	if(status == 4)
	    break;
	if(status == 2)
	    return JTAG_READ_FAILURE;
	tapc_tms(1);//exit1-dr
	tapc_tms(1);//update-dr
	/* Do not redo the transaction. Pause and re-try.
	 */
	//tapc_tms(0);//run-test-idle
    }
    if (i==arcjtag_retry_count)
      return JTAG_READ_FAILURE;

    /* JTAG status register leaves us in Shift DR */
    tapc_tms(1); /* Move to Exit-1 DR */
    tapc_tms(1); /* Move to Update DR */
    
    // data = jtag data reg
    data = read_jtag_reg(0xB, 32);

    *readbuf = data;
    
    return sizeof(data);
}

static int
arc_jtag_write_core_reg (unsigned int regnum, unsigned int data)
{
    unsigned int rd, wr, i;

    check_and_raise_error_if_jtag_closed();
    tapc_reset();
    tapc_tms(0);//run-test idle

    // data = jtag data reg
    write_jtag_reg(0xB, data, 32);//update dr

    // jtag addr register = regnum:
    write_jtag_reg(0xA, regnum, 32);//update dr
    
    

    // jtag transaction command reg = 0x1(write core reg)
    write_jtag_reg(0x9, 0x1, 4);//update dr

    /* Perform the transaction.
     */
    tapc_tms(0); // run-test idle
    
    for (i=0;i<arcjtag_retry_count;i++)
    {
      unsigned int status = read_jtag_status_reg();
      	if(status == 4)
	    break;
	if(status == 2)
	    return JTAG_WRITE_FAILURE;
	tapc_tms(1);//exit1-dr
	tapc_tms(1);//update-dr
	/* Do not redo the transaction. Pause and re-try.
	 */
	//tapc_tms(0);//run-test-idle
    }
    if (i==arcjtag_retry_count)
      return JTAG_READ_FAILURE;

    tapc_tms(1);//exit1-dr
    tapc_tms(1);//update-dr
    /* This should have been done earlier i.e. before
       reading the JTAG status register.
     */
    //tapc_tms(0);//rn-test-idle
    return sizeof(data);
}

static int
arc_jtag_read_aux_reg (unsigned int regnum, unsigned int *readbuf)
{
    unsigned int rd, wr, data, i;
    if(arc_jtag_ops.arc_jtag_state_machine_debug)
      printf_filtered("\nEntered arc_jtag_read_aux_reg()\
         \n Regnum:%d \n",regnum);
    check_and_raise_error_if_jtag_closed();
    check_and_raise_error_if_jtag_closed();
    tapc_reset();
    tapc_tms(0);//run-test idle
    write_jtag_reg(0xA, regnum, 32);//update dr



    // Setup instruction register to 0x9 indicating
    // a JTAG instruction is being downloaded.
    // jtag transaction command reg = 0x6 (read aux reg)
    write_jtag_reg(0x9, 0x6, 4);//update dr

    /* Perform the transaction.
     */
    tapc_tms(0); // run-test idle

    // poll the status

    for (i=0;i<arcjtag_retry_count;i++)
    {
	unsigned int status = read_jtag_status_reg();
	
	if(status == 4)
	    break;
	if(status == 2)
	    return JTAG_READ_FAILURE;
	tapc_tms(1);//exit1-dr
	tapc_tms(1);//update-dr
	/* Do not redo the transaction. Pause and re-try.
	 */
	//tapc_tms(0);//run-test-idle

    }
    if (i==arcjtag_retry_count)
      return JTAG_READ_FAILURE;


/*     JTAG status register leaves us in Shift DR */
    tapc_tms(1); /* Move to Exit-1 IR */
    tapc_tms(1); /* Move to Update IR */
    
    // data = jtag data reg
    data = read_jtag_reg(0xB, 32);
    *readbuf = data;
    if(arc_jtag_ops.arc_jtag_state_machine_debug)
      printf(" Data: %d\n",data);
    return sizeof(data);
}

static int
arc_jtag_write_aux_reg (unsigned int regnum, unsigned int data)
{
    unsigned int rd, wr, i;
    if(arc_jtag_ops.arc_jtag_state_machine_debug)
      printf_filtered("\nEntered arc_jtag_write_aux_reg()\n Regnum:%d\nData:%d\n",regnum,data);
    check_and_raise_error_if_jtag_closed();
    tapc_reset();
    tapc_tms(0);//run-test idle
    
    // data = jtag data reg
    write_jtag_reg(0xB, data, 32);//update dr

    // jtag addr register = regnum:
    write_jtag_reg(0xA, regnum, 32);//update dr
    
    

    // jtag transaction command reg = 0x2 (write aux reg)
    write_jtag_reg(0x9, 0x2, 4);//update dr

    /* Perform the transaction.
     */
    tapc_tms(0); // run-test idle

    for (i=0;i<arcjtag_retry_count;i++)
    {
	unsigned int status = read_jtag_status_reg();
	if(status == 4)
	    break;
	if(status == 2)
	    return JTAG_WRITE_FAILURE;
	tapc_tms(1);//exit1-dr
	tapc_tms(1);//update-dr
	/* Do not redo the transaction. Pause and re-try.
	 */
	//tapc_tms(0);//run-test-idle
    }
    if (i==arcjtag_retry_count)
      return JTAG_READ_FAILURE;

	tapc_tms(1);//exit1-dr
	tapc_tms(1);//update-dr
	/* This should have been done earlier i.e. before
	   reading the JTAG status register.
	*/
	//tapc_tms(0);//run-test-idle
	return sizeof(data);
}


static int
read_mem (unsigned int addr, unsigned int *readbuf)
{
    unsigned int rd, wr, data, i;

    rw_flag = READ_FROM_AA3;
    if(arc_jtag_ops.arc_jtag_state_machine_debug)
      printf_filtered("\nEntered read_mem() 0x%x\n",addr);
    // jtag addr register = regnum:
    tapc_reset();
    tapc_tms(0);//run-test idle
    write_jtag_reg(0xA, addr, 32);//update dr

    

    // Setup instruction register to 0x9 indicating
    // a JTAG instruction is being downloaded.
    // jtag transaction command reg = 0x5 (read core reg)
    write_jtag_reg(0x9, 0x4, 4);//update dr

    /* Perform the transaction.
     */
    tapc_tms(0); // run-test idle

    // poll the status

    for (i=0;i<arcjtag_retry_count;i++)
    {
	unsigned int status = read_jtag_status_reg();
	if(status == 4)
	    break;
	if(status == 2)
	    return JTAG_READ_FAILURE;
	tapc_tms(1);//exit1-dr
	tapc_tms(1);//update-dr
	/* Do not redo the transaction. Pause and re-try.
	 */
	//tapc_tms(0); // run-test-idle
    }
    if (i==arcjtag_retry_count)
      return JTAG_READ_FAILURE;

    /* JTAG status register leaves us in Shift DR */
    tapc_tms(1); /* Move to Exit-1 DR */
    tapc_tms(1); /* Move to Update DR */
    
    // data = jtag data reg
    data = read_jtag_reg(0xB, 32);
    *readbuf = data;
    if(arc_jtag_ops.arc_jtag_state_machine_debug)
    printf_filtered("\n Read rd =0x%x in read_mem",data);

    return sizeof(data);
}


static int
write_mem (unsigned int addr, unsigned int data)
{
    unsigned int rd, wr, i;
    if(arc_jtag_ops.arc_jtag_state_machine_debug)
      printf_filtered("\nEntered write_mem() to write 0x%x at 0x%x\n",data,addr);
    tapc_reset();
    tapc_tms(0);//run-test idle
    
    // data = jtag data reg
    write_jtag_reg(0xB, data, 32);//update dr

    // jtag addr register = regnum:
    write_jtag_reg(0xA, addr, 32);//update dr
    
    

    // jtag transaction command reg = 0x0(write mem)
    write_jtag_reg(0x9, 0x0, 4);//update dr

    /* Perform the transaction.
     */
    tapc_tms(0); // run-test idle

    for (i=0;i<arcjtag_retry_count;i++)
    {
	unsigned int status = read_jtag_status_reg();
	if(status == 4)
	    break;
	if(status == 2)
	    return JTAG_WRITE_FAILURE;
	tapc_tms(1);//exit1-dr
	tapc_tms(1);//update-dr
	/* Do not redo the transaction. Pause and re-try.
	 */
	//tapc_tms(0);//run-tes-idle
    }
    if (i==arcjtag_retry_count)
      return JTAG_READ_FAILURE;

    tapc_tms(1);//exit1-dr
    tapc_tms(1);//update-dr
    /* This should have been done earlier i.e. before
       reading the JTAG status register.
     */
    //tapc_tms(0);//run-test-idle
    return sizeof(data);
}


static int 
arc_jtag_write_chunk (unsigned int addr, unsigned int *write_buf, int len)
{
  unsigned int rd, wr;
  unsigned int status;
  check_and_raise_error_if_jtag_closed();
    
  rw_flag = WRITE_TO_AA3 ;
  int i = 0;
  int len_mod = len % 4;
  len = len - len_mod;
  
  tapc_reset();
  if(arc_jtag_ops.arc_jtag_state_machine_debug)
    printf_filtered("Entered arc_jtag_write_chunk()......0x%x\n",*write_buf);
  tapc_tms(0);//run-test idle
  
  // jtag addr register = regnum:
  write_jtag_reg(0xA, addr, 32);//update dr
  
  
  
  // jtag transaction command reg = 0x0(write mem)
  write_jtag_reg(0x9, 0x0, 4);//update dr
  
  while(len)
    {
      
      // data = jtag data reg
      write_jtag_reg(0xB, write_buf[i++], 32);//update dr
      
      /* Perform the transaction.
       */
      tapc_tms(0); // run-test idle
      
      
      while(1)
	{
	  status = read_jtag_status_reg();
	  if(status == 4)
	    break;
	  if(status == 2)
	    return (i-1) * 4;

	  tapc_tms(1);//exit1-dr
	  tapc_tms(1);//update-dr
	  /* Do not redo the transaction. Pause and re-try.
	   */
	  //tapc_tms(0);//run-tes-idle
	}
      
      tapc_tms(1);//exit1-dr
      tapc_tms(1);//update-dr
      /* This should have been done earlier i.e. before
	 reading the JTAG status register.
      */
      //tapc_tms(0);//run-test idle
      len = len - 4;
    }

  if(arc_jtag_ops.arc_jtag_state_machine_debug)
    printf_filtered("leaving arc_jtag_write_chunk() with return value %d",i*4);
  
  // added support for writing no of bytes those are not exact mutiple of four
  
  if(!len_mod)
    return i*4;
  else
    {
      
      addr=addr+(i*4);
      if(read_mem(addr,&rd)==JTAG_READ_FAILURE)
	  return i*4;
      if(arc_jtag_ops.arc_jtag_state_machine_debug)
      printf_filtered("\nrd=0x%x and wr=0x%x\n",rd,write_buf[i]);

      switch(len_mod)
	{
	  
	case 1:
	  wr = (rd & 0xffffff00)|(write_buf[i] & 0xff);
	  break;
	  
	case 2:
	  wr = (rd & 0xffff0000)|(write_buf[i] & 0xffff);
	  break;
	  
	case 3:
	  wr = (rd & 0xff000000)|(write_buf[i] & 0xffffff);
	  break;
	  
	}
      if(arc_jtag_ops.arc_jtag_state_machine_debug)
	printf_filtered("\nwrite_mem writing 0x%x at 0x%x",wr,addr);
      arc_jtag_write_chunk(addr,&wr,4);
      return (i*4 + len_mod);
    }
  
  
}

static int 
arc_jtag_read_chunk (unsigned int addr, unsigned int *read_buf, int len)
{
    unsigned int rd, wr, data;
    int i=0;
    rw_flag = READ_FROM_AA3 ;
    int len_mod=len%4;
    len=len-len_mod;
    if(arc_jtag_ops.arc_jtag_state_machine_debug)
      printf_filtered("\nEntered arc_jtag_read_chunk() 0x%x\n",addr);
    check_and_raise_error_if_jtag_closed();

    // jtag addr register = regnum:
    tapc_reset();
    tapc_tms(0);//run-test idle
    
    write_jtag_reg(0xA, addr, 32);//update dr

    
    while(len)
    {
    // Setup instruction register to 0x9 indicating
    // a JTAG instruction is being downloaded.
    // jtag transaction command reg = 0x5 (read core reg)
    write_jtag_reg(0x9, 0x4, 4);//update dr

    /* Perform the transaction.
     */
    tapc_tms(0); // run-test idle

    // poll the status

    while(1)
    {
	unsigned int status = read_jtag_status_reg();
	if(status == 4)
	    break;
	if(status == 2)
	    return (i-1)*4;
	tapc_tms(1);//exit1-dr
	tapc_tms(1);//update-dr
	/* Do not redo the transaction. Pause and re-try.
	 */
	//tapc_tms(0);//run-test-idle

    }

    /* JTAG status register leaves us in Shift DR */
    tapc_tms(1); /* Move to Exit-1 DR */
    tapc_tms(1); /* Move to Update DR */
    
    // data = jtag data reg
    read_buf[i++] = read_jtag_reg(0xB, 32);// exits in Update DR
    len= len-4;
    //tapc_tms(0);/* Move to run-test-idle */

    }

    // added support for reading no of bytes those are not exact mutiple of four

    if(!len_mod)
	return i*4;
    else
    {
        char *ptr = (char *)read_buf + i*4;
	addr=addr+(i*4);
	if(read_mem(addr,&rd)==JTAG_READ_FAILURE)
	    return i*4;

	switch(len_mod)
	{
	case 1:
	    ptr[0] = rd & 0xff;
	    break;

	case 2:
	    ptr[0] = rd & 0xff;
	    ptr[1] = (rd>>8) & 0xff;
	    break;

	case 3:
	    ptr[0] = rd & 0xff;
	    ptr[1] = (rd>>8) & 0xff;
	    ptr[2] = (rd>>16) & 0xff;
	    break;
	}

	return ((i*4)+len_mod);
	
    }


}



/*
 * Return the Processor Variant that is connected.
 */
int
arc_get_architecture() 
{
  if (ARCProcessor == UNSUPPORTED) {
    unsigned int value;

    /* Read the Identity Register. */
    if (arc_jtag_read_aux_reg(4, &value) == JTAG_READ_FAILURE)
      error("Failure reading from auxillary IDENTITY register");

    /* Get Identity Mask. */
    value &= 0xff ;

    if((value >= 0x30) && (value <= 0x3f))
      ARCProcessor = ARC700;
    else if((value >= 0x20) && (value <= 0x2f))
      ARCProcessor = ARC600;
    else if((value >= 0x10) && (value <= 0x1f))
      ARCProcessor = A5;
    else if ((value >= 0x00) && (value <= 0x0f)) 
      ARCProcessor = A4;
    else 
      error("Unsupported Processor Version 0x%x\n", value);
  }

  return ARCProcessor;
}



static void 
arc_jtag_open (void)
{
  int retval;
  unsigned int read_status = 0;
  sigaddset(&block_mask,SIGINT);
  retval = gpio_setup();
  if(retval != 0)
    {
      error("Unable to open JTAG Port .%s \n",(retval == EINVAL)?
	    "Invalid Params":"Permission Denied" );
      arc_jtag_ops.jtag_status = JTAG_CLOSED;
      return ;
    }
  arc_jtag_ops.jtag_status = JTAG_OPENED;

  tapc_reset();
  //Writing debug bit of debug register
  
  do
    {
      /* Note: Reading the status/status32 register here to
         check if halt bit is set*/
      if (IS_A4) {
	if(arc_jtag_read_aux_reg( 0x0, &read_status) == JTAG_READ_FAILURE)
	  error("Failure reading auxillary register 0xA\n");
	if(read_status & A4_HALT_VALUE)
	  break;
      }
      else {
	if(arc_jtag_read_aux_reg( 0xA, &read_status) == JTAG_READ_FAILURE)
	  error("Failure reading auxillary register 0xA\n");
	if(read_status & ARC700_HALT_VALUE)
	  break;
      }
      printf_filtered("Processor running. Trying to halt.....\n");
      if(arc_jtag_write_aux_reg(0x5,0x2)==JTAG_WRITE_FAILURE)
	error("Failure writing 0x2 to auxillary register 0x5:debug register\n");
    }while(1);
  if (arc_jtag_ops.arc_jtag_state_machine_debug)
    printf_filtered("Processor halted.....\n");

}

static void 
arc_jtag_close (void)
{
  ARCProcessor = UNSUPPORTED;

  if(arc_jtag_ops.jtag_status != JTAG_CLOSED)
    {
      tapc_reset();
      /* closing file descriptor opened for communication with gpio driver */
      close(fd);
      if(arc_jtag_ops.arc_jtag_state_machine_debug)
	printf_filtered("arc-jtag closed\n");
      arc_jtag_ops.jtag_status = JTAG_CLOSED;
    }

}

static void 
arc_jtag_wait(void)
{
  unsigned int read_status;
  check_and_raise_error_if_jtag_closed();
  do
    {
      sigprocmask(SIG_BLOCK,&block_mask, NULL);

      if (IS_A4) {
	if(arc_jtag_read_aux_reg( 0x0, &read_status) == JTAG_READ_FAILURE)
	  error("Failure reading auxillary register 0x0\n");
	//FIXMEA:    if(debug_arc_jtag_target_message)
	//      printf_filtered ("\n Read Status: 0x%x,%d\n", read_status, read_status);

	if(read_status & A4_HALT_VALUE)
	  {
	    sigprocmask(SIG_BLOCK,&block_mask, NULL);
	    break;
	  }
      }
      else {
	if(arc_jtag_read_aux_reg( 0xA, &read_status) == JTAG_READ_FAILURE)
	  error("Failure reading auxillary register 0xA\n");
	if(read_status & ARC700_HALT_VALUE)
	  {
	    sigprocmask(SIG_BLOCK,&block_mask, NULL);
	    break;

	  }
      }
      sigprocmask(SIG_UNBLOCK,&block_mask, NULL);	
    }while(1);

  while (1)
    {
      if (arc_jtag_read_aux_reg (0x5,&read_status) == JTAG_READ_FAILURE)
	error ("Failure reading Debug register \n");
      if (!(read_status & 0x80000000))
	break;
    }
}

static void
arc_jtag_reset_board (void)
{
  char c = 5;
  int  auxval = 2 ;
  check_and_raise_error_if_jtag_closed ();

  /*
    Writing 9 did not work. But thats 
    what the manual says. Hmmm. 
  gpio_write (CONTROL_PORT, &c);
  */

  gpio_write ( CONTROL_PORT, &c);
  c = 0xd;
  gpio_write ( CONTROL_PORT, &c);
  c = 5;
  gpio_write ( CONTROL_PORT, &c);

  if (arc_jtag_write_aux_reg(0x5 , 2) == JTAG_WRITE_FAILURE)
    error ("Failure writing to auxiliary register Debug\n");

  c = 0;
  gpio_write ( 0x378, &c);
  c = 0x40;
  gpio_write ( 0x378, &c);
  c = 0;
  gpio_write ( 0x378, &c);
  
  tapc_reset();
}


void
_initialize_arc_jtag_ops (void)
{
    arc_jtag_ops.name=NULL;
    arc_jtag_ops.jtag_open = arc_jtag_open;
    arc_jtag_ops.jtag_close = arc_jtag_close;
    arc_jtag_ops.jtag_memory_write = arc_jtag_write_chunk;
    arc_jtag_ops.jtag_memory_read = arc_jtag_read_chunk;
    arc_jtag_ops.jtag_memory_chunk_write = arc_jtag_write_chunk;
    arc_jtag_ops.jtag_memory_chunk_read = arc_jtag_read_chunk;
    arc_jtag_ops.jtag_write_aux_reg = arc_jtag_write_aux_reg;
    arc_jtag_ops.jtag_read_aux_reg = arc_jtag_read_aux_reg;
    arc_jtag_ops.jtag_read_core_reg = arc_jtag_read_core_reg;
    arc_jtag_ops.jtag_write_core_reg = arc_jtag_write_core_reg;
    arc_jtag_ops.jtag_wait = arc_jtag_wait;
    arc_jtag_ops.jtag_reset_board = arc_jtag_reset_board;
    arc_jtag_ops.jtag_status = JTAG_CLOSED ;
}
