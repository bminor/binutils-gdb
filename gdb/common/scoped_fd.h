/* scoped_fd, automatically close a file descriptor

   Copyright (C) 2018 Free Software Foundation, Inc.

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

#ifndef SCOPED_FD_H
#define SCOPED_FD_H

#include "config.h"

#ifdef HAVE_UNISTD_H

#include <unistd.h>

/* A smart-pointer-like class to automatically close a file descriptor.  */

class scoped_fd
{
public:
  explicit scoped_fd (int fd = -1) noexcept : m_fd (fd) {}
  ~scoped_fd ()
  {
    if (m_fd >= 0)
      close (m_fd);
  }

  DISABLE_COPY_AND_ASSIGN (scoped_fd);

  int release () noexcept
  {
    int fd = m_fd;
    m_fd = -1;
    return fd;
  }

  int get () const noexcept
  {
    return m_fd;
  }

private:
  int m_fd;
};

#endif /* HAVE_UNISTD_H */
#endif /* SCOPED_FD_H */
