/* Intel I860 opcde list for GDB, the GNU debugger.
   Copyright (C) 1992 Free Software Foundation, Inc.
   Contributed by Peggy Fieland (pfieland@stratus.com)

GDB is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the GDB General Public License for full details.

Everyone is granted permission to copy, modify and redistribute GDB,
but only under the conditions described in the GDB General Public
License.  A copy of this license is supposed to have been given to you
along with GDB so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

In other words, go ahead and share GDB, but don't try to stop
anyone else from sharing it farther.  Help stamp out software hoarding!
*/

#ifdef BIG_ENDIAN
struct gen_fmt
{
  unsigned op1 : 6;
  unsigned src2 : 5;
  unsigned dest : 5;
  unsigned src1 : 5;
  unsigned offset : 11;
};

struct geni_fmt
{
  unsigned op1 : 6;
  unsigned src2 : 5;
  unsigned dest : 5;
  unsigned offset : 16;
};

struct esc_fmt
{
  unsigned op1 : 6;
  unsigned res1 : 10;
  unsigned src1 : 5;
  unsigned res2 : 6;
  unsigned op2 : 5;
};
struct ctrl_fmt
{
  unsigned op1 : 6;
  unsigned int offset : 26;
};

struct fp_fmt
{
  unsigned op1 : 6;
  unsigned src2 : 5;
  unsigned dest : 5;
  unsigned src1 : 5;
  unsigned p : 1;
  unsigned d : 1;
  unsigned s : 1;
  unsigned r : 1;
  unsigned op2 : 7;
};

union insn_fmt
{
  struct gen_fmt gen;
  struct geni_fmt geni;
  struct esc_fmt esc;
  struct ctrl_fmt ctrl;
  struct fp_fmt	fp;
  long int_val;
};
#else
struct gen_fmt
{
  unsigned offset : 11;
  unsigned src1 : 5;
  unsigned dest : 5;
  unsigned src2 : 5;
  unsigned op1 : 6;
};

struct geni_fmt
{
  unsigned offset : 16;
  unsigned dest : 5;
  unsigned src2 : 5;
  unsigned op1 : 6;
};

struct esc_fmt
{
  unsigned op2 : 5;
  unsigned res2 : 6;
  unsigned src1 : 5;
  unsigned res1 : 10;
  unsigned op1 : 6;
};
struct ctrl_fmt
{
  unsigned int offset : 26;
  unsigned op1 : 6;
};

struct fp_fmt
{
  unsigned op2 : 7;
  unsigned r : 1;
  unsigned s : 1;
  unsigned d : 1;
  unsigned p : 1;
  unsigned src1 : 5;
  unsigned dest : 5;
  unsigned src2 : 5;
  unsigned op1 : 6;
};

union insn_fmt
{
  struct gen_fmt gen;
  struct geni_fmt geni;
  struct esc_fmt esc;
  struct ctrl_fmt ctrl;
  struct fp_fmt	fp;
  long int_val;
};
#endif

typedef enum
{
  Error, not_branch, uncond, uncond_d, cond, cond_d
} branch_type;


