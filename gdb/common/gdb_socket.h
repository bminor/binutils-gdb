/* Copyright (C) 2015 Free Software Foundation, Inc.

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

#ifndef GDB_SOCKET_H
#define GDB_SOCKET_H

#if USE_WIN32API
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#if HAVE_SYS_UN_H
#include <sys/un.h>
#endif
#endif

/* Use this union instead of casts between struct sockaddr <-> struct
   sockaddr_foo to avoid strict aliasing violations.  */

union gdb_sockaddr_u
{
  struct sockaddr sa;
  struct sockaddr_in sa_in;
#if HAVE_SYS_UN_H
  struct sockaddr_un sa_un;
#endif
};

#endif /* GDB_SOCKET_H */
