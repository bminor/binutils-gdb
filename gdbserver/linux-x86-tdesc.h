/* Low level support for x86 (i386 and x86-64), shared between gdbserver
   and IPA.

   Copyright (C) 2016-2024 Free Software Foundation, Inc.

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

#ifndef GDBSERVER_LINUX_X86_TDESC_H
#define GDBSERVER_LINUX_X86_TDESC_H

/* Convert an xcr0 value into an integer.  The integer will be passed to
   the in-process-agent where it will then be passed to
   x86_linux_tdesc_idx_to_xcr0 to get back the xcr0 value.  */

extern int x86_linux_xcr0_to_tdesc_idx (uint64_t xcr0);

#ifdef IN_PROCESS_AGENT

/* Convert an index number (as returned from x86_linux_xcr0_to_tdesc_idx)
   into an xcr0 value which can then be used to create a target
   description.  */

extern uint64_t x86_linux_tdesc_idx_to_xcr0 (int idx);

/* Within the in-process-agent we need to pre-initialise all of the target
   descriptions, to do this we need to know how many target descriptions
   there are for each different target type.  These functions return the
   target description count for the relevant target.  */

extern int x86_linux_amd64_ipa_tdesc_count ();
extern int x86_linux_x32_ipa_tdesc_count ();
extern int x86_linux_i386_ipa_tdesc_count ();


#endif /* IN_PROCESS_AGENT */

#endif /* GDBSERVER_LINUX_X86_TDESC_H */
