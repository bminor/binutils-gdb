/* Remote Serial support interface definitions for GDB, the GNU Debugger.
   Copyright 1992 Free Software Foundation, Inc.

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

/* Return a sensible default name for a serial device, something which
   can be used as an argument to serial_open  */
   
CONST char *EXFUN(serial_default_name,(void));

/* Try to open the serial device "name", return 1 if ok, 0 if not. */

int EXFUN(serial_open,(CONST char *name));

/* Turn the port into raw mode */

void EXFUN(serial_raw,(void));


/* Turn the port into normal mode */

void EXFUN(serial_normal,(void));


/* Read one char from the serial device with timeout, return char, and
   set ok if ok */

int EXFUN(serial_timedreadchar,(int to , int *ok));


/* Set the baudrate to the value supplied, and return 1, or fail and
   return 0 */

int EXFUN(serial_setbaudrate,(int to));

/* Return the next rate in the sequence, or return 0 for a fail*/

int EXFUN(serial_nextbaudrate,(int rate));


/* Write some chars to the device, return 1 if ok, 0 if not */

int EXFUN(serial_write,( CONST char *str ,  int len));


/* Close the serial port */

int EXFUN(serial_close,(void));
