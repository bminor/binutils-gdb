/* Remote serial support interface definitions for GDB, the GNU Debugger.
   Copyright 1992, 1993 Free Software Foundation, Inc.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Terminal state pointer.  This is specific to each type of interface. */

typedef PTR ttystate;

struct _serial_t
{
  int fd;
  struct serial_ops *ops;
  ttystate ttystate;
  int bufcnt;
  unsigned char *bufp;
  unsigned char buf[BUFSIZ];
};

typedef struct _serial_t *serial_t;

struct serial_ops {
  char *name;
  struct serial_ops *next;
  int (*open) PARAMS ((serial_t, const char *name));
  void (*close) PARAMS ((serial_t));
  int (*readchar) PARAMS ((serial_t, int timeout));
  int (*write) PARAMS ((serial_t, const char *str, int len));
  void (*go_raw) PARAMS ((serial_t));
  void (*restore) PARAMS ((serial_t));
  int (*setbaudrate) PARAMS ((serial_t, int rate));
};

/* Add a new serial interface to the interface list */

void serial_add_interface PARAMS ((struct serial_ops *optable));

/* Try to open the serial device "name", returns a serial_t if ok, NULL if not.
 */

serial_t serial_open PARAMS ((const char *name));

/* Internal open routine for specific I/O interface */

#define SERIAL_OPEN(SERIAL_T, NAME) (SERIAL_T)->ops->open((SERIAL_T), NAME)

/* Turn the port into raw mode.  */

#define SERIAL_RAW(SERIAL_T) (SERIAL_T)->ops->go_raw((SERIAL_T))

/* Read one char from the serial device with <TO>-second timeout.
   Returns char if ok, else EOF, -2 for timeout, -3 for anything else  */

#define SERIAL_READCHAR(SERIAL_T, TIMEOUT) ((SERIAL_T)->ops->readchar((SERIAL_T), TIMEOUT))

/* Set the baudrate to the decimal value supplied.  Return 1 on failure,
   0 otherwise.  */

#define SERIAL_SETBAUDRATE(SERIAL_T, RATE) ((SERIAL_T)->ops->setbaudrate((SERIAL_T), RATE))

/* Write some chars to the device, returns 0 for failure.  See errno for
   details. */

#define SERIAL_WRITE(SERIAL_T, STRING, LEN) ((SERIAL_T)->ops->write((SERIAL_T), STRING, LEN))

/* Close the serial port */

#define SERIAL_CLOSE(SERIAL_T) (SERIAL_T)->ops->close((SERIAL_T))

/* Restore the serial port to the state saved in oldstate */

#define SERIAL_RESTORE(SERIAL_T) (SERIAL_T)->ops->restore((SERIAL_T))
