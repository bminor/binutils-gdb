/* Sparc opcde list for GDB, the GNU debugger.
   Copyright (C) 1986, 1987 Free Software Foundation, Inc.
   Contributed by Michael Tiemann (tiemann@mcc.com)

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

struct op1_fmt
{
  unsigned op1 : 2;
  unsigned dummy : 30;
};

struct op2_fmt
{
  unsigned dummy1 : 7;
  unsigned op2 : 3;
  unsigned dummy2 : 22;
};

struct op3_fmt
{
  unsigned dummy1 : 7;
  unsigned op3 : 6;
  unsigned dummy2 : 19;
};

struct call_fmt
{
  unsigned op : 2;
  unsigned disp : 30;
};

struct sethi_fmt
{
  unsigned op : 2;
  unsigned rd : 5;
  unsigned op2 : 3;
  unsigned imm : 22;
};

struct branch_fmt
{
  unsigned op : 2;
  unsigned a : 1;
  unsigned cond : 4;
  unsigned op2 : 3;
  unsigned disp : 22;		/* this should really be signed.  */
};

struct ldst_fmt
{
  unsigned op : 2;
  unsigned rd : 5;
  unsigned op3 : 6;
  unsigned rs1 : 5;
  unsigned i : 1;
  unsigned asi : 8;
  unsigned rs2 : 5;
};

struct arith_imm_fmt
{
  unsigned op : 2;
  unsigned rd : 5;
  unsigned op3 : 6;
  unsigned rs1 : 5;
  unsigned i : 1;
  int simm : 13;		/* Hopefully signed, but . . . */
};

struct arith_fmt
{
  unsigned op : 2;
  unsigned rd : 5;
  unsigned op3 : 6;
  unsigned rs1 : 5;
  unsigned i : 1;
  unsigned opf : 8;
  unsigned rs2 : 5;
};

union insn_fmt
{
  struct op1_fmt op1;
  struct op2_fmt op2;
  struct op3_fmt op3;
  struct call_fmt call;
  struct sethi_fmt sethi;
  struct branch_fmt branch;
  struct ldst_fmt ldst;
  struct arith_imm_fmt arith_imm;
  struct arith_fmt arith;
  int intval;
  float floatval;		/* ?? */
};

typedef enum
{
  Error, not_branch, bicc, bicca, ba, baa, ticc, ta,
} branch_type;

