/* Register test program.

   Copyright 2007 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

void
read_regs (unsigned long long regs[16], unsigned long control_regs[6])
{
  asm volatile ("wstrd wr0, %0" : "=m" (regs[0]));
  asm volatile ("wstrd wr1, %0" : "=m" (regs[1]));
  asm volatile ("wstrd wr2, %0" : "=m" (regs[2]));
  asm volatile ("wstrd wr3, %0" : "=m" (regs[3]));
  asm volatile ("wstrd wr4, %0" : "=m" (regs[4]));
  asm volatile ("wstrd wr5, %0" : "=m" (regs[5]));
  asm volatile ("wstrd wr6, %0" : "=m" (regs[6]));
  asm volatile ("wstrd wr7, %0" : "=m" (regs[7]));
  asm volatile ("wstrd wr8, %0" : "=m" (regs[8]));
  asm volatile ("wstrd wr9, %0" : "=m" (regs[9]));
  asm volatile ("wstrd wr10, %0" : "=m" (regs[10]));
  asm volatile ("wstrd wr11, %0" : "=m" (regs[11]));
  asm volatile ("wstrd wr12, %0" : "=m" (regs[12]));
  asm volatile ("wstrd wr13, %0" : "=m" (regs[13]));
  asm volatile ("wstrd wr14, %0" : "=m" (regs[14]));
  asm volatile ("wstrd wr15, %0" : "=m" (regs[15]));

  asm volatile ("wstrw wcssf, %0" : "=m" (control_regs[0]));
  asm volatile ("wstrw wcasf, %0" : "=m" (control_regs[1]));
  asm volatile ("wstrw wcgr0, %0" : "=m" (control_regs[2]));
  asm volatile ("wstrw wcgr1, %0" : "=m" (control_regs[3]));
  asm volatile ("wstrw wcgr2, %0" : "=m" (control_regs[4]));
  asm volatile ("wstrw wcgr3, %0" : "=m" (control_regs[5]));
}

void
write_regs (unsigned long long regs[16], unsigned long control_regs[6])
{
  asm volatile ("wldrd wr0, %0" : : "m" (regs[0]));
  asm volatile ("wldrd wr1, %0" : : "m" (regs[1]));
  asm volatile ("wldrd wr2, %0" : : "m" (regs[2]));
  asm volatile ("wldrd wr3, %0" : : "m" (regs[3]));
  asm volatile ("wldrd wr4, %0" : : "m" (regs[4]));
  asm volatile ("wldrd wr5, %0" : : "m" (regs[5]));
  asm volatile ("wldrd wr6, %0" : : "m" (regs[6]));
  asm volatile ("wldrd wr7, %0" : : "m" (regs[7]));
  asm volatile ("wldrd wr8, %0" : : "m" (regs[8]));
  asm volatile ("wldrd wr9, %0" : : "m" (regs[9]));
  asm volatile ("wldrd wr10, %0" : : "m" (regs[10]));
  asm volatile ("wldrd wr11, %0" : : "m" (regs[11]));
  asm volatile ("wldrd wr12, %0" : : "m" (regs[12]));
  asm volatile ("wldrd wr13, %0" : : "m" (regs[13]));
  asm volatile ("wldrd wr14, %0" : : "m" (regs[14]));
  asm volatile ("wldrd wr15, %0" : : "m" (regs[15]));

  asm volatile ("wldrw wcssf, %0" : : "m" (control_regs[0]));
  asm volatile ("wldrw wcasf, %0" : : "m" (control_regs[1]));
  asm volatile ("wldrw wcgr0, %0" : : "m" (control_regs[2]));
  asm volatile ("wldrw wcgr1, %0" : : "m" (control_regs[3]));
  asm volatile ("wldrw wcgr2, %0" : : "m" (control_regs[4]));
  asm volatile ("wldrw wcgr3, %0" : : "m" (control_regs[5]));
}

int
main ()
{
  unsigned long long regs[16];
  unsigned long control_regs[6];

  read_regs (regs, control_regs);
  write_regs (regs, control_regs);

  return 0;
}
