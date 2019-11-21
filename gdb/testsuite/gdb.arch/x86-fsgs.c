/* This testcase is part of GDB, the GNU debugger.

   Copyright 2019 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <asm/ldt.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/mman.h>

#if HAVE_ARCH_SET_FS || HAVE_ARCH_SET_GS
#  include <asm/prctl.h>
#  include <sys/prctl.h>
#endif /* HAVE_ARCH_SET_FS || HAVE_ARCH_SET_GS */

struct segs {
  int initial;
  int other;
  int twentythree;
};
static struct segs *segs;

static unsigned int
setup_ldt (unsigned int entry, void *base, size_t size)
{
    struct user_desc ud;
    int errcode;

    memset (&ud, 0, sizeof (ud));
    ud.entry_number = entry;
    ud.base_addr = (unsigned int) (unsigned long) base;
    ud.limit = (unsigned int) size;

    /* The base is 32-bit.  */
    if ((unsigned long) ud.base_addr != (unsigned long) base)
      return 0u;

    errcode = syscall(SYS_modify_ldt, 1, &ud, sizeof(ud));
    if (errcode != 0)
      return 0u;

    return (ud.entry_number << 3) | 7;
}

int
read_fs (void)
{
  int value;

  __asm__ volatile ("mov %%fs:0x0, %0" : "=rm"(value) :: "memory");

  return value;
}

int
read_gs (void)
{
  int value;

  __asm__ volatile ("mov %%gs:0x0, %0" : "=rm"(value) :: "memory");

  return value;
}

int
switch_fs_read (unsigned int fs)
{
  __asm__ volatile ("mov %0, %%fs" :: "rm"(fs) : "memory");

  return read_fs ();
}

void
test_fs (unsigned int selector)
{
  int value;

  value = switch_fs_read (selector);	/* l.1 */
  value = read_fs ();			/* l.2 */
  value = read_fs ();			/* l.3 */
}					/* l.4 */

int
switch_gs_read (unsigned int gs)
{
  __asm__ volatile ("mov %0, %%gs" :: "rm"(gs) : "memory");

  return read_gs ();
}

void
test_gs (unsigned int selector)
{
  int value;

  value = switch_gs_read (selector);	/* l.1 */
  value = read_gs ();			/* l.2 */
  value = read_gs ();			/* l.3 */
}					/* l.4 */

#if HAVE_WRFSGSBASE

int
wrfsbase_read (void *fsbase)
{
  __asm__ volatile ("wrfsbase %0" :: "r"(fsbase) : "memory");

  return read_fs ();
}

static void
test_wrfsbase (void *base)
{
  int value;

  value = wrfsbase_read (base);	/* l.1 */
  value = read_fs ();		/* l.2 */
  value = read_fs ();		/* l.3 */
}				/* l.4 */

int
wrgsbase_read (void *gsbase)
{
  __asm__ volatile ("wrgsbase %0" :: "r"(gsbase) : "memory");

  return read_gs ();
}

static void
test_wrgsbase (void *base)
{
  int value;

  value = wrgsbase_read (base);	/* l.1 */
  value = read_gs ();		/* l.2 */
  value = read_gs ();		/* l.3 */
}				/* l.4 */

#endif /* HAVE_WRFSGSBASE */

#if HAVE_ARCH_SET_FS

int
arch_set_fs_read (void *fsbase)
{
  int errcode;

  errcode = syscall (SYS_arch_prctl, ARCH_SET_FS, fsbase);
  if (errcode != 0)
    return 0;

  return read_fs ();
}

static void
test_arch_set_fs (void *base)
{
  int value;

  value = arch_set_fs_read (base);	/* l.1 */
  value = read_fs ();			/* l.2 */
  value = read_fs ();			/* l.3 */
}					/* l.4 */

#endif /* HAVE_ARCH_SET_FS */

#if HAVE_ARCH_SET_GS

int
arch_set_gs_read (void *gsbase)
{
  int errcode;

  errcode = syscall (SYS_arch_prctl, ARCH_SET_GS, gsbase);
  if (errcode != 0)
    return 0;

  return read_gs ();
}

static void
test_arch_set_gs (void *base)
{
  int value;

  value = arch_set_gs_read (base);	/* l.1 */
  value = read_gs ();			/* l.2 */
  value = read_gs ();			/* l.3 */
}					/* l.4 */

#endif /* HAVE_ARCH_SET_GS */

int
main (void)
{
  unsigned int selector;

  segs = mmap (NULL, sizeof (*segs), PROT_READ | PROT_WRITE,
#ifdef __x86_64__
	       MAP_32BIT |
#endif
	       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (segs == MAP_FAILED)
    {
      perror ("failed to mmap 32-bit memory");
      abort ();
    }

  segs->initial = 42;
  segs->other = -42;
  segs->twentythree = 23;

  selector = setup_ldt (0xb7 >> 3, &segs->other, sizeof (segs->other));
  if (selector == 0u)
    {
      perror ("failed to setup LDT[0xb7>>3] = &segs->other");
      abort ();
    }

  selector = setup_ldt (0xa7 >> 3, &segs->initial, sizeof (segs->initial));
  if (selector == 0u)
    {
      perror ("failed to setup LDT[0xa7>>3] = &segs->initial");
      abort ();
    }

    test_fs (selector);
    test_gs (selector);

#if HAVE_ARCH_SET_FS
  test_arch_set_fs (&segs->initial);
#endif /* HAVE_ARCH_SET_FS */

#if HAVE_ARCH_SET_GS
  test_arch_set_gs (&segs->initial);
#endif /* HAVE_ARCH_SET_GS */

#if HAVE_WRFSGSBASE
  test_wrfsbase (&segs->initial);
  test_wrgsbase (&segs->initial);
#endif /* HAVE_WRFSGSBASE */

  return 0;
}
