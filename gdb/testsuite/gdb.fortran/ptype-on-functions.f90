! Copyright 2019 Free Software Foundation, Inc.
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

module some_module
  implicit none

  type, public :: Number
     integer :: a
   contains
     procedure :: get => get_number
     procedure :: set => set_number
  end type Number

contains

  function get_number (this) result (val)
    class (Number), intent (in) :: this
    integer :: val
    val = this%a
  end function get_number

  subroutine set_number (this, val)
    class (Number), intent (inout) :: this
    integer :: val
    this%a = val
  end subroutine set_number

end module some_module

logical function is_bigger (a,b)
  integer, intent(in) :: a
  integer, intent(in) :: b
  is_bigger = a > b
end function is_bigger

subroutine say_numbers (v1,v2,v3)
  integer,intent(in) :: v1
  integer,intent(in) :: v2
  integer,intent(in) :: v3
  print *, v1,v2,v3
end subroutine say_numbers

program test
  use some_module

  interface
     integer function fun1 (x)
       integer :: x
     end function fun1

     integer function fun2 (x)
       integer :: x
     end function fun2
  end interface

  type (Number) :: n1
  type (Number) :: n2

  procedure(fun1), pointer:: fun_ptr => NULL()

  call say_numbers (1,2,3)	! stop here
  print *, fun_ptr (3)

end program test

integer function fun1 (x)
  implicit none
  integer :: x
  fun1 = x + 1
end function fun1

integer function fun2 (x)
  implicit none
  integer :: x
  fun2 = x + 2
end function fun2

