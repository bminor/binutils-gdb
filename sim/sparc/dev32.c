/* sparc device support
   Copyright (C) 1999 Cygnus Solutions.  */

#include "sim-main.h"
#include "dev32.h"

#ifdef HAVE_DV_SOCKSER
#include "dv-sockser.h"
#endif

/* ??? At present this is to match erc32 so outbyte works.  */

device sparc_devices;

int
device_io_read_buffer (device *me, void *source, int space,
		       address_word addr, unsigned nr_bytes,
		       SIM_CPU *cpu, sim_cia cia)
{
  SIM_DESC sd = CPU_STATE (cpu);

  if (STATE_ENVIRONMENT (sd) != OPERATING_ENVIRONMENT)
    return nr_bytes;

#ifdef HAVE_DV_SOCKSER
  if (addr == UART_INCHAR_ADDR)
    {
      int c = dv_sockser_read (sd);
      if (c == -1)
	return 0;
      *(char *) source = c;
      return 1;
    }
  if (addr == UART_STATUS_ADDR)
    {
      int status = dv_sockser_status (sd);
      unsigned char *p = source;
      p[0] = 0;
      p[1] = (((status & DV_SOCKSER_INPUT_EMPTY)
#ifdef UART_INPUT_READY0
	       ? UART_INPUT_READY : 0)
#else
	       ? 0 : UART_INPUT_READY)
#endif
	      + ((status & DV_SOCKSER_OUTPUT_EMPTY) ? UART_OUTPUT_READY : 0));
      return 2;
    }
#endif

  /* erc32 compatibility */
  if (addr == RXSTAT)
    {
      int Ucontrol = 0;
      unsigned char *p = source;
      Ucontrol |= 1;
      Ucontrol |= 0x00060006;
      SETTSI (p, Ucontrol);
    }

  return nr_bytes;
}

int
device_io_write_buffer (device *me, const void *source, int space,
			address_word addr, unsigned nr_bytes,
			SIM_CPU *cpu, sim_cia cia)
{
  SIM_DESC sd = CPU_STATE (cpu);

#if 0 && WITH_SCACHE
  if (addr == MCCR_ADDR)
    {
      if ((*(const char *) source & MCCR_CP) != 0)
	scache_flush (sd);
      return nr_bytes;
    }
#endif

  if (STATE_ENVIRONMENT (sd) != OPERATING_ENVIRONMENT)
    return nr_bytes;

#ifdef HAVE_DV_SOCKSER
  if (addr == UART_OUTCHAR_ADDR)
    {
      int rc = dv_sockser_write (sd, *(char *) source);
      return rc == 1;
    }
#endif

  if (addr == RXADATA)
    {
      sim_io_printf (sd, "%c", GETTSI (source));
    }

  return nr_bytes;
}

void device_error () {}
