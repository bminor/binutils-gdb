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

  integer, parameter :: b1_o = 127 + 1
  integer, parameter :: b2_o = 32767 + 3
  integer*8, parameter :: b4_o = 2147483647 + 5

  integer, allocatable :: array_1d_1byte_overflow (:)
  integer, allocatable :: array_1d_2bytes_overflow (:)
  integer, allocatable :: array_1d_4bytes_overflow (:)
  integer, allocatable :: array_2d_1byte_overflow (:,:)
  integer, allocatable :: array_2d_2bytes_overflow (:,:)
  integer, allocatable :: array_3d_1byte_overflow (:,:,:)

  ! Loop counters.
  integer :: s1, s2

  allocate (array_1d_1byte_overflow (1:b1_o))
  allocate (array_1d_2bytes_overflow (1:b2_o))
  allocate (array_1d_4bytes_overflow (1:b4_o))

  allocate (array_2d_1byte_overflow (1:b1_o, 1:b1_o))
  allocate (array_2d_2bytes_overflow (1:b2_o, 1:b2_o))

  allocate (array_3d_1byte_overflow (1:b1_o, 1:b1_o, 1:b1_o))


  ! The start of the tests.
  call test_size_4 (size (array_1d))
  call test_size_4 (size (array_1d, 1))
  do s1=1, SIZE (array_1d, 1), 1
     call test_size_4 (size (array_1d (1:10:s1)))
     call test_size_4 (size (array_1d (1:10:s1), 1))
     call test_size_4 (size (array_1d (10:1:-s1)))
     call test_size_4 (size (array_1d (10:1:-s1), 1))
  end do

  do s2=1, SIZE (array_2d, 2), 1
     do s1=1, SIZE (array_2d, 1), 1
        call test_size_4 (size (array_2d (1:4:s1, 1:3:s2)))
        call test_size_4 (size (array_2d (4:1:-s1, 1:3:s2)))
        call test_size_4 (size (array_2d (1:4:s1, 3:1:-s2)))
        call test_size_4 (size (array_2d (4:1:-s1, 3:1:-s2)))

        call test_size_4 (size (array_2d (1:4:s1, 1:3:s2), 1))
        call test_size_4 (size (array_2d (4:1:-s1, 1:3:s2), 1))
        call test_size_4 (size (array_2d (1:4:s1, 3:1:-s2), 1))
        call test_size_4 (size (array_2d (4:1:-s1, 3:1:-s2), 1))

        call test_size_4 (size (array_2d (1:4:s1, 1:3:s2), 2))
        call test_size_4 (size (array_2d (4:1:-s1, 1:3:s2), 2))
        call test_size_4 (size (array_2d (1:4:s1, 3:1:-s2), 2))
        call test_size_4 (size (array_2d (4:1:-s1, 3:1:-s2), 2))
     end do
  end do

  allocate (allocatable_array_1d (-10:-5))
  call test_size_4 (size (allocatable_array_1d))
  do s1=1, SIZE (allocatable_array_1d, 1), 1
     call test_size_4 (size (allocatable_array_1d (-10:-5:s1)))
     call test_size_4 (size (allocatable_array_1d (-5:-10:-s1)))

     call test_size_4 (size (allocatable_array_1d (-10:-5:s1), 1))
     call test_size_4 (size (allocatable_array_1d (-5:-10:-s1), 1))
  end do

  allocate (allocatable_array_2d (-3:3, 8:12))
  do s2=1, SIZE (allocatable_array_2d, 2), 1
     do s1=1, SIZE (allocatable_array_2d, 1), 1
        call test_size_4 (size (allocatable_array_2d (-3:3:s1, 8:12:s2)))
        call test_size_4 (size (allocatable_array_2d (3:-3:-s1, 8:12:s2)))
        call test_size_4 (size (allocatable_array_2d (-3:3:s1, 12:8:-s2)))
        call test_size_4 (size (allocatable_array_2d (3:-3:-s1, 12:8:-s2)))

        call test_size_4 (size (allocatable_array_2d (-3:3:s1, 8:12:s2), 1))
        call test_size_4 (size (allocatable_array_2d (3:-3:-s1, 8:12:s2), 2))
        call test_size_4 (size (allocatable_array_2d (-3:3:s1, 12:8:-s2), 1))
        call test_size_4 (size (allocatable_array_2d (3:-3:-s1, 12:8:-s2), 2))
     end do
  end do

  array_1d_p => array_1d
  call test_size_4 (size (array_1d_p))
  call test_size_4 (size (array_1d_p, 1))

  array_2d_p => array_2d
  call test_size_4 (size (array_2d_p))
  call test_size_4 (size (array_2d_p, 1))
  call test_size_4 (size (array_2d_p, 2))

  ! Test kind parameters - compiler requires these to be compile time constant
  ! so sadly there cannot be a loop over the kinds 1, 2, 4, 8.
  call test_size_4 (size (array_1d_1byte_overflow))
  call test_size_4 (size (array_1d_2bytes_overflow))

  call test_size_4 (size (array_1d_1byte_overflow, 1))
  call test_size_4 (size (array_1d_2bytes_overflow, 1))

  call test_size_4 (size (array_1d_4bytes_overflow))
  call test_size_4 (size (array_1d_4bytes_overflow, 1))

  call test_size_4 (size (array_2d_1byte_overflow, 1))
  call test_size_4 (size (array_2d_1byte_overflow, 2))
  call test_size_4 (size (array_2d_2bytes_overflow, 1))
  call test_size_4 (size (array_2d_2bytes_overflow, 2))

  call test_size_4 (size (array_3d_1byte_overflow, 1))
  call test_size_4 (size (array_3d_1byte_overflow, 2))
  call test_size_4 (size (array_3d_1byte_overflow, 3))

  ! Kind 1.

  call test_size_1 (size (array_1d_1byte_overflow, 1, 1))
  call test_size_1 (size (array_1d_2bytes_overflow, 1, 1))
  call test_size_1 (size (array_1d_4bytes_overflow, 1, 1))

  call test_size_1 (size (array_2d_1byte_overflow, 1, 1))
  call test_size_1 (size (array_2d_1byte_overflow, 2, 1))
  call test_size_1 (size (array_2d_2bytes_overflow, 1, 1))
  call test_size_1 (size (array_2d_2bytes_overflow, 2, 1))

  call test_size_1 (size (array_3d_1byte_overflow, 1, 1))
  call test_size_1 (size (array_3d_1byte_overflow, 2, 1))
  call test_size_1 (size (array_3d_1byte_overflow, 3, 1))

  ! Kind 2.
  call test_size_2 (size (array_1d_1byte_overflow, 1, 2))
  call test_size_2 (size (array_1d_2bytes_overflow, 1, 2))
  call test_size_2 (size (array_1d_4bytes_overflow, 1, 2))

  call test_size_2 (size (array_2d_1byte_overflow, 1, 2))
  call test_size_2 (size (array_2d_1byte_overflow, 2, 2))
  call test_size_2 (size (array_2d_2bytes_overflow, 1, 2))
  call test_size_2 (size (array_2d_2bytes_overflow, 2, 2))

  call test_size_2 (size (array_3d_1byte_overflow, 1, 2))
  call test_size_2 (size (array_3d_1byte_overflow, 2, 2))
  call test_size_2 (size (array_3d_1byte_overflow, 3, 2))

  ! Kind 4.
  call test_size_4 (size (array_1d_1byte_overflow, 1, 4))
  call test_size_4 (size (array_1d_2bytes_overflow, 1, 4))
  call test_size_4 (size (array_1d_4bytes_overflow, 1, 4))

  call test_size_4 (size (array_2d_1byte_overflow, 1, 4))
  call test_size_4 (size (array_2d_1byte_overflow, 2, 4))
  call test_size_4 (size (array_2d_2bytes_overflow, 1, 4))
  call test_size_4 (size (array_2d_2bytes_overflow, 2, 4))

  call test_size_4 (size (array_3d_1byte_overflow, 1, 4))
  call test_size_4 (size (array_3d_1byte_overflow, 2, 4))
  call test_size_4 (size (array_3d_1byte_overflow, 3, 4))

  ! Kind 8.
  call test_size_8 (size (array_1d_1byte_overflow, 1, 8))
  call test_size_8 (size (array_1d_2bytes_overflow, 1, 8))
  call test_size_8 (size (array_1d_4bytes_overflow, 1, 8))

  call test_size_8 (size (array_2d_1byte_overflow, 1, 8))
  call test_size_8 (size (array_2d_1byte_overflow, 2, 8))
  call test_size_8 (size (array_2d_2bytes_overflow, 1, 8))
  call test_size_8 (size (array_2d_2bytes_overflow, 2, 8))

  call test_size_8 (size (array_3d_1byte_overflow, 1, 8))
  call test_size_8 (size (array_3d_1byte_overflow, 2, 8))
  call test_size_8 (size (array_3d_1byte_overflow, 3, 8))

  print *, "" ! Breakpoint before deallocate.

  deallocate (allocatable_array_1d)
  deallocate (allocatable_array_2d)

  deallocate (array_3d_1byte_overflow)

  deallocate (array_2d_2bytes_overflow)
  deallocate (array_2d_1byte_overflow)

  deallocate (array_1d_4bytes_overflow)
  deallocate (array_1d_2bytes_overflow)
  deallocate (array_1d_1byte_overflow)

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
  subroutine test_size_1 (answer)
    integer*1 :: answer

    print *, answer	! Test Breakpoint 1
  end subroutine test_size_1

  subroutine test_size_2 (answer)
    integer*2 :: answer

    print *, answer	! Test Breakpoint 2
  end subroutine test_size_2

  subroutine test_size_4 (answer)
    integer*4 :: answer

    print *, answer	! Test Breakpoint 3
  end subroutine test_size_4

  subroutine test_size_8 (answer)
    integer*8 :: answer

    print *, answer	! Test Breakpoint 4
  end subroutine test_size_8

end program test
