/* Target-dependent code for GNU/Linux, architecture independent.

   Copyright (C) 2009 Free Software Foundation, Inc.

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
#include "gdbtypes.h"
#include "linux-tdep.h"

/* This function is suitable for architectures that don't
   extend/override the standard siginfo structure.  */

struct type *
linux_get_siginfo_type (struct gdbarch *gdbarch)
{
  struct type *int_type, *uint_type, *long_type, *void_ptr_type;
  struct type *uid_type, *pid_type;
  struct type *sigval_type, *clock_type;
  struct type *siginfo_type, *sifields_type;
  struct type *type;

  int_type = init_type (TYPE_CODE_INT,
			gdbarch_int_bit (gdbarch) / HOST_CHAR_BIT,
			0, "int", NULL);
  uint_type = init_type (TYPE_CODE_INT,
			 gdbarch_int_bit (gdbarch) / HOST_CHAR_BIT,
			 0, "unsigned int", NULL);
  long_type = init_type (TYPE_CODE_INT,
			 gdbarch_long_bit (gdbarch) / HOST_CHAR_BIT,
			 0, "long", NULL);
  void_ptr_type = lookup_pointer_type (builtin_type (gdbarch)->builtin_void);

  /* sival_t */
  sigval_type = init_composite_type (NULL, TYPE_CODE_UNION);
  TYPE_NAME (sigval_type) = xstrdup ("sigval_t");
  append_composite_type_field (sigval_type, "sival_int", int_type);
  append_composite_type_field (sigval_type, "sival_ptr", void_ptr_type);

  /* __pid_t */
  pid_type = init_type (TYPE_CODE_TYPEDEF, TYPE_LENGTH (int_type),
			TYPE_FLAG_TARGET_STUB, NULL, NULL);
  TYPE_NAME (pid_type) = xstrdup ("__pid_t");
  TYPE_TARGET_TYPE (pid_type) = int_type;

  /* __uid_t */
  uid_type = init_type (TYPE_CODE_TYPEDEF, TYPE_LENGTH (uint_type),
			TYPE_FLAG_TARGET_STUB, NULL, NULL);
  TYPE_NAME (uid_type) = xstrdup ("__uid_t");
  TYPE_TARGET_TYPE (uid_type) = uint_type;

  /* __clock_t */
  clock_type = init_type (TYPE_CODE_TYPEDEF, TYPE_LENGTH (long_type),
			  TYPE_FLAG_TARGET_STUB, NULL, NULL);
  TYPE_NAME (clock_type) = xstrdup ("__clock_t");
  TYPE_TARGET_TYPE (clock_type) = long_type;

  /* _sifields */
  sifields_type = init_composite_type (NULL, TYPE_CODE_UNION);

  {
    const int si_max_size = 128;
    int si_pad_size;
    int size_of_int = gdbarch_int_bit (gdbarch) / HOST_CHAR_BIT;

    /* _pad */
    if (gdbarch_ptr_bit (gdbarch) == 64)
      si_pad_size = (si_max_size / size_of_int) - 4;
    else
      si_pad_size = (si_max_size / size_of_int) - 3;
    append_composite_type_field (sifields_type, "_pad",
				 init_vector_type (int_type, si_pad_size));
  }

  /* _kill */
  type = init_composite_type (NULL, TYPE_CODE_STRUCT);
  append_composite_type_field (type, "si_pid", pid_type);
  append_composite_type_field (type, "si_uid", uid_type);
  append_composite_type_field (sifields_type, "_kill", type);

  /* _timer */
  type = init_composite_type (NULL, TYPE_CODE_STRUCT);
  append_composite_type_field (type, "si_tid", int_type);
  append_composite_type_field (type, "si_overrun", int_type);
  append_composite_type_field (type, "si_sigval", sigval_type);
  append_composite_type_field (sifields_type, "_timer", type);

  /* _rt */
  type = init_composite_type (NULL, TYPE_CODE_STRUCT);
  append_composite_type_field (type, "si_pid", pid_type);
  append_composite_type_field (type, "si_uid", uid_type);
  append_composite_type_field (type, "si_sigval", sigval_type);
  append_composite_type_field (sifields_type, "_rt", type);

  /* _sigchld */
  type = init_composite_type (NULL, TYPE_CODE_STRUCT);
  append_composite_type_field (type, "si_pid", pid_type);
  append_composite_type_field (type, "si_uid", uid_type);
  append_composite_type_field (type, "si_status", int_type);
  append_composite_type_field (type, "si_utime", clock_type);
  append_composite_type_field (type, "si_stime", clock_type);
  append_composite_type_field (sifields_type, "_sigchld", type);

  /* _sigfault */
  type = init_composite_type (NULL, TYPE_CODE_STRUCT);
  append_composite_type_field (type, "si_addr", void_ptr_type);
  append_composite_type_field (sifields_type, "_sigfault", type);

  /* _sigpoll */
  type = init_composite_type (NULL, TYPE_CODE_STRUCT);
  append_composite_type_field (type, "si_band", long_type);
  append_composite_type_field (type, "si_fd", int_type);
  append_composite_type_field (sifields_type, "_sigpoll", type);

  /* struct siginfo */
  siginfo_type = init_composite_type (NULL, TYPE_CODE_STRUCT);
  TYPE_NAME (siginfo_type) = xstrdup ("siginfo");
  append_composite_type_field (siginfo_type, "si_signo", int_type);
  append_composite_type_field (siginfo_type, "si_errno", int_type);
  append_composite_type_field (siginfo_type, "si_code", int_type);
  append_composite_type_field_aligned (siginfo_type,
				       "_sifields", sifields_type,
				       TYPE_LENGTH (long_type));

  return siginfo_type;
}
