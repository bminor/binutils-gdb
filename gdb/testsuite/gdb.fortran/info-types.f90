! Copyright 2019 Free Software Foundation, Inc.
!
! This program is free software; you can redistribute it and/or modify
! it under the terms of the GNU General Public License as published by
! the Free Software Foundation; either version 2 of the License, or
! (at your option) any later version.
!
! This program is distributed in the hope that it will be useful,
! but WITHOUT ANY WARRANTY; without even the implied warranty of
! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
! GNU General Public License for more details.
!
! You should have received a copy of the GNU General Public License
! along with this program.  If not, see <http://www.gnu.org/licenses/>.

module mod1
  type :: m1t1
     integer :: b
  end type m1t1
end module mod1

program info_types_test
  use mod1

  type :: s1
     integer :: a
  end type s1

  logical :: l
  type (s1) :: var_a
  type (m1t1) :: var_b

  var_a%a = 1
  var_b%b = 2
  l = .FALSE.
end program info_types_test
