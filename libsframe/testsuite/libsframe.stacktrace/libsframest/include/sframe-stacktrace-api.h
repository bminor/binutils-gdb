/* Public API to SFrame stacktracer.

   Copyright (C) 2023 Free Software Foundation, Inc.

   This file is part of libsframest.

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

#ifndef	_SFRAME_BACKTRACE_API_H
#define	_SFRAME_BACKTRACE_API_H

#ifdef	__cplusplus
extern "C"
{
#endif

enum sframe_bt_errcode
{
  SFRAME_BT_OK,
  SFRAME_BT_ERR_BAD_SFSTATE,
  SFRAME_BT_ERR_ARG,
  SFRAME_BT_ERR_MALLOC,
  SFRAME_BT_ERR_REALLOC,
  SFRAME_BT_ERR_OPEN,
  SFRAME_BT_ERR_READLINK,
  SFRAME_BT_ERR_LSEEK,
  SFRAME_BT_ERR_READ,
  SFRAME_BT_ERR_GETCONTEXT,
  SFRAME_BT_ERR_DECODE,
  SFRAME_BT_ERR_CFA_OFFSET,
  SFRAME_BT_ERR_FRE_INVAL,
};

#define SFRAME_BT_ERR ((int) -1)

#define NUM_OF_DSOS	32			/* Number of DSOs.  */

#define ENUM_ARRAY_SIZE(a) (sizeof (a) / sizeof ((a)[0]))

/* Get the stacktrace of the calling program by storing return addresses
   in BUFFER. The SIZE argument specifies the maximum number of addresses
   that can be stored in the buffer. Return the number of return addresses
   collected or -1 if there is any error.  */
extern int sframe_stacktrace (void **buffer, int size, int *errp);

extern const char *sframe_bt_errmsg (enum sframe_bt_errcode ecode);

extern int sframe_bt_ret_set_errno (int *errp, int error);

extern int sframe_bt_errno (const int *errp);

#ifdef	__cplusplus
}
#endif

#endif				/* _SFRAME_BACKTRACE_API_H */
