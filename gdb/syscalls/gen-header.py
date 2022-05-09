# Copyright (C) 2013-2022 Free Software Foundation, Inc.

# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

import sys
import time

infname = sys.argv[1]

print(
    """\
<?xml version="1.0"?>
<!-- Copyright (C) 2009-%s Free Software Foundation, Inc.

     Copying and distribution of this file, with or without modification,
     are permitted in any medium without royalty provided the copyright
     notice and this notice are preserved.  -->

<!DOCTYPE feature SYSTEM "gdb-syscalls.dtd">

<!-- This file was generated using the following file:

     %s

     The file mentioned above belongs to the Linux Kernel.  -->

"""
    % (time.strftime("%Y"), infname)
)
