! Copyright 2021-2022 Free Software Foundation, Inc.
!
! This program is free software; you can redistribute it and/or modify
! it under the terms of the GNU General Public License as published by
! the Free Software Foundation; either version 3 of the License, or
! (at your option) any later version.
!
! This program is distributed in the hope that it will be useful,
! but WITHOUT ANY WARRANTY; without even the implied warranty of
! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
! GNU General Public License for more details.
!
! You should have received a copy of the GNU General Public License
! along with this program.  If not, see <http://www.gnu.org/licenses/>.

!
! Start of test program.
!
program test

  ! Things to perform tests on.
  integer, target :: array_1d (1:10) = 0
  integer, target :: array_2d (1:4, 1:3) = 0
  integer :: an_integer = 0
  real :: a_real = 0.0
  integer, pointer :: array_1d_p (:) => null ()
  integer, pointer :: array_2d_p (:,:) => null ()
  integer, allocatable :: allocatable_array_1d (:)
  integer, allocatable :: allocatable_array_2d (:,:)

  ! Loop counters.
  integer :: s1, s2

  ! The start of the tests.
  call test_size (size (array_1d))
  call test_size (size (array_1d, 1))
  do s1=1, SIZE (array_1d, 1), 1
     call test_size (size (array_1d (1:10:s1)))
     call test_size (size (array_1d (1:10:s1), 1))
     call test_size (size (array_1d (10:1:-s1)))
     call test_size (size (array_1d (10:1:-s1), 1))
  end do

  do s2=1, SIZE (array_2d, 2), 1
     do s1=1, SIZE (array_2d, 1), 1
        call test_size (size (array_2d (1:4:s1, 1:3:s2)))
        call test_size (size (array_2d (4:1:-s1, 1:3:s2)))
        call test_size (size (array_2d (1:4:s1, 3:1:-s2)))
        call test_size (size (array_2d (4:1:-s1, 3:1:-s2)))

        call test_size (size (array_2d (1:4:s1, 1:3:s2), 1))
        call test_size (size (array_2d (4:1:-s1, 1:3:s2), 1))
        call test_size (size (array_2d (1:4:s1, 3:1:-s2), 1))
        call test_size (size (array_2d (4:1:-s1, 3:1:-s2), 1))

        call test_size (size (array_2d (1:4:s1, 1:3:s2), 2))
        call test_size (size (array_2d (4:1:-s1, 1:3:s2), 2))
        call test_size (size (array_2d (1:4:s1, 3:1:-s2), 2))
        call test_size (size (array_2d (4:1:-s1, 3:1:-s2), 2))
     end do
  end do

  allocate (allocatable_array_1d (-10:-5))
  call test_size (size (allocatable_array_1d))
  do s1=1, SIZE (allocatable_array_1d, 1), 1
     call test_size (size (allocatable_array_1d (-10:-5:s1)))
     call test_size (size (allocatable_array_1d (-5:-10:-s1)))

     call test_size (size (allocatable_array_1d (-10:-5:s1), 1))
     call test_size (size (allocatable_array_1d (-5:-10:-s1), 1))
  end do

  allocate (allocatable_array_2d (-3:3, 8:12))
  do s2=1, SIZE (allocatable_array_2d, 2), 1
     do s1=1, SIZE (allocatable_array_2d, 1), 1
        call test_size (size (allocatable_array_2d (-3:3:s1, 8:12:s2)))
        call test_size (size (allocatable_array_2d (3:-3:-s1, 8:12:s2)))
        call test_size (size (allocatable_array_2d (-3:3:s1, 12:8:-s2)))
        call test_size (size (allocatable_array_2d (3:-3:-s1, 12:8:-s2)))

        call test_size (size (allocatable_array_2d (-3:3:s1, 8:12:s2), 1))
        call test_size (size (allocatable_array_2d (3:-3:-s1, 8:12:s2), 2))
        call test_size (size (allocatable_array_2d (-3:3:s1, 12:8:-s2), 1))
        call test_size (size (allocatable_array_2d (3:-3:-s1, 12:8:-s2), 2))
     end do
  end do

  array_1d_p => array_1d
  call test_size (size (array_1d_p))
  call test_size (size (array_1d_p, 1))

  array_2d_p => array_2d
  call test_size (size (array_2d_p))
  call test_size (size (array_2d_p, 1))
  call test_size (size (array_2d_p, 2))

  deallocate (allocatable_array_1d)
  deallocate (allocatable_array_2d)
  array_1d_p => null ()
  array_2d_p => null ()

  print *, "" ! Final Breakpoint
  print *, an_integer
  print *, a_real
  print *, associated (array_1d_p)
  print *, associated (array_2d_p)
  print *, allocated (allocatable_array_1d)
  print *, allocated (allocatable_array_2d)

contains

  subroutine test_size (answer)
    integer :: answer

    print *,answer	! Test Breakpoint
  end subroutine test_size

end program test
