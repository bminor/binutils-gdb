/* Self tests for gdbarch for GDB, the GNU debugger.

   Copyright (C) 2017-2020 Free Software Foundation, Inc.

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
#include "gdbsupport/selftest.h"
#include "selftest-arch.h"
#include "inferior.h"
#include "gdbthread.h"
#include "target.h"
#include "test-target.h"
#include "target-float.h"
#include "gdbsupport/def-vector.h"
#include "gdbarch.h"
#include "progspace-and-thread.h"

namespace selftests {

/* Test gdbarch methods register_to_value and value_to_register.  */

static void
register_to_value_test (struct gdbarch *gdbarch)
{
  const struct builtin_type *builtin = builtin_type (gdbarch);
  struct type *types[] =
    {
      builtin->builtin_void,
      builtin->builtin_char,
      builtin->builtin_short,
      builtin->builtin_int,
      builtin->builtin_long,
      builtin->builtin_signed_char,
      builtin->builtin_unsigned_short,
      builtin->builtin_unsigned_int,
      builtin->builtin_unsigned_long,
      builtin->builtin_float,
      builtin->builtin_double,
      builtin->builtin_long_double,
      builtin->builtin_complex,
      builtin->builtin_double_complex,
      builtin->builtin_string,
      builtin->builtin_bool,
      builtin->builtin_long_long,
      builtin->builtin_unsigned_long_long,
      builtin->builtin_int8,
      builtin->builtin_uint8,
      builtin->builtin_int16,
      builtin->builtin_uint16,
      builtin->builtin_int32,
      builtin->builtin_uint32,
      builtin->builtin_int64,
      builtin->builtin_uint64,
      builtin->builtin_int128,
      builtin->builtin_uint128,
      builtin->builtin_char16,
      builtin->builtin_char32,
    };

  /* Create a mock environment.  An inferior with a thread, with a
     process_stratum target pushed.  */

  test_target_ops mock_target;
  ptid_t mock_ptid (1, 1);
  program_space mock_pspace (new_address_space ());
  inferior mock_inferior (mock_ptid.pid ());
  mock_inferior.gdbarch = gdbarch;
  mock_inferior.aspace = mock_pspace.aspace;
  mock_inferior.pspace = &mock_pspace;
  thread_info mock_thread (&mock_inferior, mock_ptid);

  scoped_restore_current_pspace_and_thread restore_pspace_thread;

  scoped_restore restore_thread_list
    = make_scoped_restore (&mock_inferior.thread_list, &mock_thread);

  /* Add the mock inferior to the inferior list so that look ups by
     target+ptid can find it.  */
  scoped_restore restore_inferior_list
    = make_scoped_restore (&inferior_list, &mock_inferior);

  /* Switch to the mock inferior.  */
  switch_to_inferior_no_thread (&mock_inferior);

  /* Push the process_stratum target so we can mock accessing
     registers.  */
  push_target (&mock_target);

  /* Pop it again on exit (return/exception).  */
  SCOPE_EXIT { pop_all_targets_at_and_above (process_stratum); };

  /* Switch to the mock thread.  */
  switch_to_thread (&mock_thread);

  struct frame_info *frame = get_current_frame ();
  const int num_regs = gdbarch_num_cooked_regs (gdbarch);

  /* Test gdbarch methods register_to_value and value_to_register with
     different combinations of register numbers and types.  */
  for (const auto &type : types)
    {
      for (auto regnum = 0; regnum < num_regs; regnum++)
	{
	  if (gdbarch_convert_register_p (gdbarch, regnum, type))
	    {
	      std::vector<gdb_byte> expected (TYPE_LENGTH (type), 0);

	      if (type->code () == TYPE_CODE_FLT)
		{
		  /* Generate valid float format.  */
		  target_float_from_string (expected.data (), type, "1.25");
		}
	      else
		{
		  for (auto j = 0; j < expected.size (); j++)
		    expected[j] = (regnum + j) % 16;
		}

	      gdbarch_value_to_register (gdbarch, frame, regnum, type,
					 expected.data ());

	      /* Allocate two bytes more for overflow check.  */
	      std::vector<gdb_byte> buf (TYPE_LENGTH (type) + 2, 0);
	      int optim, unavail, ok;

	      /* Set the fingerprint in the last two bytes.  */
	      buf [TYPE_LENGTH (type)]= 'w';
	      buf [TYPE_LENGTH (type) + 1]= 'l';
	      ok = gdbarch_register_to_value (gdbarch, frame, regnum, type,
					      buf.data (), &optim, &unavail);

	      SELF_CHECK (ok);
	      SELF_CHECK (!optim);
	      SELF_CHECK (!unavail);

	      SELF_CHECK (buf[TYPE_LENGTH (type)] == 'w');
	      SELF_CHECK (buf[TYPE_LENGTH (type) + 1] == 'l');

	      for (auto k = 0; k < TYPE_LENGTH(type); k++)
		SELF_CHECK (buf[k] == expected[k]);
	    }
	}
    }
}

} // namespace selftests

void _initialize_gdbarch_selftests ();
void
_initialize_gdbarch_selftests ()
{
  selftests::register_test_foreach_arch ("register_to_value",
					 selftests::register_to_value_test);
}
