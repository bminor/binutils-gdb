#include <nwtypes.h>
#include "alpha-patch.h"
#include <errno.h>
#include <stdio.h>

#define CONST const

/* This file provides stubs and equivalent interfaces for all functions that
   the debugger stub needs, but aren't yet implemented. */

int
AIOReadData (int portHandle, char  *buffer, LONG length, LONG *numberBytesRead)
{
  ULONG c;

  while (1)
    {
      c = com_interrupt ();
      if ((c & ~0377) == COM1)
	break;
    }

  *buffer = c;
  *numberBytesRead = 1;

  return AIO_SUCCESS;
}

int
AIOWriteData (int portHandle, char *buffer, LONG length,
	      LONG *numberBytesWritten)
	
{
  *numberBytesWritten = length;

  while (length-- > 0)
    uart_putchar (COM1, *buffer++);

  return AIO_SUCCESS;
}

int
AIOAcquirePort (int *hardwareType, int *boardNumber, int *portNumber,
		int *portHandle)
{
  return AIO_SUCCESS;
}

int
AIOConfigurePort (int portHandle, BYTE bitRate,	BYTE dataBits, BYTE stopBits,
		  BYTE parityMode, BYTE flowCtrlMode)
{
  uart_init_line (1, 9600);

  return AIO_SUCCESS;
}

int
AIOGetPortConfiguration (int portHandle, AIOPORTCONFIG *pPortConfig,
			     AIODVRCONFIG *pDvrConfig)
{
  fprintf (stderr, "AIOGetPortConfiguration stubbed out\n");
  exit (1);
}

int
AIOReleasePort (int portHandle)
{
  return AIO_SUCCESS;
}
	
int
AIOSetExternalControl (int portHandle, int requestType, int requestValue)
{
  return AIO_SUCCESS;
}

int
AIOGetExternalStatus (int portHandle, LONG *extStatus, LONG *chgdExtStatus)
{
  fprintf (stderr, "AIOGetExternalStatus stubbed out\n");
  exit (1);
}

void
StopBell ()
{
}

int
Breakpoint (int __arg)
{
  fprintf (stderr, "Breakpoint() stubbed out\n");
  exit (1);
}

/*
 * strtol : convert a string to long.
 *
 * Andy Wilson, 2-Oct-89.
 */

/* FIXME: It'd be nice to configure around these, but the include files are too
   painful.  These macros should at least be more portable than hardwired hex
   constants. */

#define	ULONG_MAX	((unsigned long)(~0L))		/* 0xFFFFFFFF */
#define	LONG_MAX	((long)(ULONG_MAX >> 1))	/* 0x7FFFFFFF */
#define	LONG_MIN	((long)(~LONG_MAX))		/* 0x80000000 */

extern int errno;

long
strtol(s, ptr, base)
     CONST char *s; char **ptr; int base;
{
  extern unsigned long  strtoul();
  int minus=0;
  unsigned long tmp;
  CONST char *start=s, *eptr;

  if (s==NULL)
    {
      errno = ERANGE;
      if (!ptr)
	*ptr = (char *)start;
      return 0L;
    }
  while (isspace(*s))
	s++;
  if (*s == '-') {
	s++;
	minus = 1;
      }
  else if (*s == '+')
    s++;

  /*
   * let strtoul do the hard work.
   */
  tmp = strtoul(s, &eptr, base);
  if (ptr != NULL)
    *ptr = (char *)((eptr==s) ? (char *)start : eptr);
  if (tmp > (minus ? - (unsigned long) LONG_MIN : (unsigned long) LONG_MAX))
    {
      errno = ERANGE;
      return (minus ? LONG_MIN : LONG_MAX);
    }
  return (minus ? (long) -tmp : (long) tmp);
}

/*
 * strtol : convert a string to long.
 *
 * Andy Wilson, 2-Oct-89.
 */

#ifndef ULONG_MAX
#define	ULONG_MAX	((unsigned long)(~0L))		/* 0xFFFFFFFF */
#endif

extern int errno;

unsigned long
strtoul(s, ptr, base)
     CONST char *s; char **ptr; int base;
{
  unsigned long total = 0;
  unsigned digit;
  CONST char *start=s;
  int did_conversion=0;
  int overflow = 0;
  int negate = 0;
  unsigned long maxdiv, maxrem;

  if (s==NULL)
    {
      errno = ERANGE;
      if (!ptr)
	*ptr = (char *)start;
      return 0L;
    }

  while (isspace(*s))
    s++;
  if (*s == '+')
    s++;
  else if (*s == '-')
    s++, negate = 1;
  if (base==0 || base==16) /*  the 'base==16' is for handling 0x */
    {
      int tmp;

      /*
       * try to infer base from the string
       */
      if (*s != '0')
        tmp = 10;	/* doesn't start with 0 - assume decimal */
      else if (s[1] == 'X' || s[1] == 'x')
	tmp = 16, s += 2; /* starts with 0x or 0X - hence hex */
      else
	tmp = 8;	/* starts with 0 - hence octal */
      if (base==0)
	base = (int)tmp;
    }

  maxdiv = ULONG_MAX / base;
  maxrem = ULONG_MAX % base;

  while ((digit = *s) != '\0')
    {
      if (digit >= '0' && digit < ('0'+base))
	digit -= '0';
      else
	if (base > 10)
	  {
	    if (digit >= 'a' && digit < ('a'+(base-10)))
	      digit = digit - 'a' + 10;
	    else if (digit >= 'A' && digit < ('A'+(base-10)))
	      digit = digit - 'A' + 10;
	    else
	      break;
	  }
	else
	  break;
      did_conversion = 1;
      if (total > maxdiv
	  || (total == maxdiv && digit > maxrem))
	overflow = 1;
      total = (total * base) + digit;
      s++;
    }
  if (overflow)
    {
      errno = ERANGE;
      if (ptr != NULL)
	*ptr = (char *)s;
      return (ULONG_MAX);
    }
  if (ptr != NULL)
    *ptr = (char *) ((did_conversion) ? (char *)s : (char *)start);
  return negate ? -total : total;
}
