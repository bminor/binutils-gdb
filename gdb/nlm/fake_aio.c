#include <nwtypes.h>
#include <errno.h>
#include <stdio.h>
#include <aio.h>

#define CONST const

/* This file provides stubs and equivalent interfaces for all functions that
   the debugger stub needs, but aren't yet implemented. */

int
AIOReadData (int portHandle, char  *buffer, LONG length, LONG *numberBytesRead)
{
  return AIO_SUCCESS;
}

int
AIOWriteData (int portHandle, char *buffer, LONG length,
	      LONG *numberBytesWritten)
	
{
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
