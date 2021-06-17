/* Support for ignoring signals.

   Copyright (C) 2021 Free Software Foundation, Inc.

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

#ifndef SCOPED_IGNORE_SIGNAL_H
#define SCOPED_IGNORE_SIGNAL_H

#include <signal.h>

/* RAII class used to ignore a signal in a scope.  */

template <int Sig>
class scoped_ignore_signal
{
public:
  scoped_ignore_signal ()
  {
    m_osig = signal (Sig, SIG_IGN);
  }

  ~scoped_ignore_signal ()
  {
    signal (Sig, m_osig);
  }

  DISABLE_COPY_AND_ASSIGN (scoped_ignore_signal);

private:
  sighandler_t m_osig = nullptr;
};

struct scoped_ignore_signal_nop
{
  /* Note, these can't both be "= default", because otherwise the
     compiler warns that variables of this type are not used.  */
  scoped_ignore_signal_nop ()
  {}
  ~scoped_ignore_signal_nop ()
  {}
  DISABLE_COPY_AND_ASSIGN (scoped_ignore_signal_nop);
};

#ifdef SIGPIPE
using scoped_ignore_sigpipe = scoped_ignore_signal<SIGPIPE>;
#else
using scoped_ignore_sigpipe = scoped_ignore_signal_nop;
#endif

#endif /* SCOPED_IGNORE_SIGNAL_H */
