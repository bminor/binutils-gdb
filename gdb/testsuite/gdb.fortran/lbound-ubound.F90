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

#define DO_TEST(ARRAY)	\
  call do_test (lbound (ARRAY), ubound (ARRAY))

subroutine do_test (lb, ub)
  integer, dimension (:) :: lb
  integer, dimension (:) :: ub

  print *, ""
  print *, "Expected GDB Output:"
  print *, ""

  write(*, fmt="(A)", advance="no") "LBOUND = ("
  do i=LBOUND (lb, 1), UBOUND (lb, 1), 1
     if (i > LBOUND (lb, 1)) then
        write(*, fmt="(A)", advance="no") ", "
     end if
     write(*, fmt="(I0)", advance="no") lb (i)
  end do
  write(*, fmt="(A)", advance="yes") ")"

  write(*, fmt="(A)", advance="no") "UBOUND = ("
  do i=LBOUND (ub, 1), UBOUND (ub, 1), 1
     if (i > LBOUND (ub, 1)) then
        write(*, fmt="(A)", advance="no") ", "
     end if
     write(*, fmt="(I0)", advance="no") ub (i)
  end do
  write(*, fmt="(A)", advance="yes") ")"

  print *, ""	! Test Breakpoint
end subroutine do_test

!
! Start of test program.
!
program test
  interface
     subroutine do_test (lb, ub)
       integer, dimension (:) :: lb
       integer, dimension (:) :: ub
     end subroutine do_test
  end interface

  ! Declare variables used in this test.
  integer, dimension (-8:-1,-10:-2) :: neg_array
  integer, dimension (2:10,1:9), target :: array
  integer, allocatable :: other (:, :)
  character (len=26) :: str_1 = "abcdefghijklmnopqrstuvwxyz"
  integer, dimension (-2:2,-3:3,-1:5) :: array3d
  integer, dimension (-3:3,7:10,-4:2,-10:-7) :: array4d
  integer, dimension (10:20) :: array1d
  integer, dimension(:,:), pointer :: pointer2d => null()
  integer, dimension(-2:6,-1:9), target :: tarray
  integer :: an_int

  integer, dimension (:), pointer :: pointer1d => null()

  ! Allocate or associate any variables as needed.
  allocate (other (-5:4, -2:7))
  pointer2d => tarray
  pointer1d => array (3, 2:5)

  DO_TEST (neg_array)
  DO_TEST (neg_array (-7:-3,-5:-4))
  DO_TEST (array)
  ! The following is disabled due to a bug in gfortran:
  !   https://gcc.gnu.org/bugzilla/show_bug.cgi?id=99027
  ! gfortran generates the incorrect expected results.
  ! DO_TEST (array (3, 2:5))
  DO_TEST (pointer1d)
  DO_TEST (other)
  DO_TEST (array3d)
  DO_TEST (array4d)
  DO_TEST (array1d)
  DO_TEST (pointer2d)
  DO_TEST (tarray)

  ! All done.  Deallocate.
  deallocate (other)

  ! GDB catches this final breakpoint to indicate the end of the test.
  print *, "" ! Final Breakpoint.

  ! Reference otherwise unused locals in order to keep them around.
  ! GDB will make use of these for some tests.
  print *, str_1
  an_int = 1
  print *, an_int

end program test
