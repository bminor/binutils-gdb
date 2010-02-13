# icf_safe_so_test.sh -- test --icf=safe

# Copyright 2010 Free Software Foundation, Inc.
# Written by Sriraman Tallam <tmsriram@google.com>.

# This file is part of gold.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
# MA 02110-1301, USA.

# The goal of this program is to verify if --icf=safe  works as expected.
# File icf_safe_so_test.cc is in this test.  The goal of this script is
# to verify if identical code folding in safe mode correctly folds
# functions in a shared object.

check_nofold()
{
    func_addr_1=`grep $2 $1 | awk '{print $1}'`
    func_addr_2=`grep $3 $1 | awk '{print $1}'`
    if [ $func_addr_1 = $func_addr_2 ];
    then
        echo "Safe Identical Code Folding folded" $2 "and" $3
	exit 1
    fi
}

check_fold()
{
    func_addr_1=`grep $2 $1 | awk '{print $1}'`
    func_addr_2=`grep $3 $1 | awk '{print $1}'`
    if [ $func_addr_1 != $func_addr_2 ];
    then
        echo "Safe Identical Code Folding did not fold " $2 "and" $3
	exit 1
    fi
}

arch_specific_safe_fold()
{
    grep_x86_64=`grep -q "Advanced Micro Devices X86-64" $2`
    if [ $? == 0 ];
    then
      check_fold $1 $3 $4
    else
      check_nofold $1 $3 $4
    fi
}

check_nofold icf_safe_so_test_1.stdout "foo_prot" "foo_hidden"
check_nofold icf_safe_so_test_1.stdout "foo_prot" "foo_internal"
check_nofold icf_safe_so_test_1.stdout "foo_prot" "foo_static"
check_nofold icf_safe_so_test_1.stdout "foo_hidden" "foo_internal"
check_nofold icf_safe_so_test_1.stdout "foo_hidden" "foo_static"
check_nofold icf_safe_so_test_1.stdout "foo_internal" "foo_static"
arch_specific_safe_fold icf_safe_so_test_1.stdout icf_safe_so_test_2.stdout \
  "foo_glob" "bar_glob"

