/* Frame info pointer

   Copyright (C) 2022 Free Software Foundation, Inc.

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

#include "defs.h"

#include "frame-info.h"
#include "frame.h"

/* See frame-info-ptr.h.  */

intrusive_list<frame_info_ptr> frame_info_ptr::frame_list;

/* See frame-info-ptr.h.  */

void
frame_info_ptr::prepare_reinflate ()
{
  m_cached_id = get_frame_id (*this);
}

/* See frame-info-ptr.h.  */

void
frame_info_ptr::reinflate ()
{
  gdb_assert (frame_id_p (m_cached_id));

  if (m_ptr == nullptr)
    m_ptr = frame_find_by_id (m_cached_id).get ();
  gdb_assert (m_ptr != nullptr);
}
