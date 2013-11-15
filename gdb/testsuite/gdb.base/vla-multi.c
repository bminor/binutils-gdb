/* This testcase is part of GDB, the GNU debugger.

   Copyright 2014 Free Software Foundation, Inc.

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

void
f1 (int n, int m, int vla_ptr[n][m])
{
  return;                                 /* f1_breakpoint */
}

void
vla_mult (int n, int m)
{
  int vla[n][m];
  int i, j;

  for (i = 0; i < n; i++)
    {
      for (j = 0; j < m; j++)
        {
          vla[i][j] = i + j;
        }
    }

  f1(n, m, vla);                          /* vla_filled */

  return;
}

int
main(void)
{
  vla_mult(2, 2);
  return 0;
}
