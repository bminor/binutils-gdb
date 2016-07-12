! Copyright 2016 Free Software Foundation, Inc.
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

program vla_strings
  character(len=:), target, allocatable   :: var_char
  character(len=:), pointer               :: var_char_p
  logical                                 :: l

  allocate(character(len=10) :: var_char)
  l = allocated(var_char)                 ! var_char-allocated-1
  var_char = 'foo'
  deallocate(var_char)                    ! var_char-filled-1
  l = allocated(var_char)                 ! var_char-deallocated
  allocate(character(len=42) :: var_char)
  l = allocated(var_char)
  var_char = 'foobar'
  var_char = ''                           ! var_char-filled-2
  var_char = 'bar'                        ! var_char-empty
  deallocate(var_char)
  allocate(character(len=21) :: var_char)
  l = allocated(var_char)                 ! var_char-allocated-3
  var_char = 'johndoe'
  var_char_p => var_char
  l = associated(var_char_p)              ! var_char_p-associated
  var_char_p => null()
  l = associated(var_char_p)              ! var_char_p-not-associated
end program vla_strings
