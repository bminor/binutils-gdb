/* scoped_mmap, automatically unmap files

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

#ifndef SCOPED_MMAP_H
#define SCOPED_MMAP_H

#include "config.h"

#ifdef HAVE_SYS_MMAN_H

#include <sys/mman.h>

/* A smart-pointer-like class to mmap() and automatically munmap() a memory
   mapping.  */

class scoped_mmap
{
public:
  scoped_mmap () noexcept : m_mem (MAP_FAILED), m_length (0) {}
  scoped_mmap (void *addr, size_t length, int prot, int flags, int fd,
	       off_t offset) noexcept : m_length (length)
    {
      m_mem = mmap (addr, m_length, prot, flags, fd, offset);
    }
  ~scoped_mmap ()
    {
      if (m_mem != MAP_FAILED)
	munmap (m_mem, m_length);
    }

  DISABLE_COPY_AND_ASSIGN (scoped_mmap);

  void *release () noexcept
    {
      void *mem = m_mem;
      m_mem = MAP_FAILED;
      m_length = 0;
      return mem;
    }

  void reset (void *addr, size_t length, int prot, int flags, int fd,
	      off_t offset) noexcept
    {
      if (m_mem != MAP_FAILED)
	munmap (m_mem, m_length);

      m_length = length;
      m_mem = mmap (addr, m_length, prot, flags, fd, offset);
    }

  size_t size () const noexcept { return m_length; }
  void *get () const noexcept { return m_mem; }

private:
  void *m_mem;
  size_t m_length;
};

#endif /* HAVE_SYS_MMAN_H */
#endif /* SCOPED_MMAP_H */
