! Copyright 2015 Free Software Foundation, Inc.
!
! Contributed by Intel Corp. <christoph.t.weinmann@intel.com>
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

subroutine sub
  integer, dimension(9) :: ar1
  integer, dimension(9,9) :: ar2
  integer, dimension(9,9,9) :: ar3
  integer, dimension(10,-7:3, -15:-5) :: ar4
  integer :: i,j,k

  ar1 = 1
  ar2 = 1
  ar3 = 1
  ar4 = 4

  ! Resulting array ar3 looks like ((( 111, 112, 113, 114,...)))
  do i = 1, 9, 1
    ar1(i) = i
    do j = 1, 9, 1
      ar2(i,j) = i*10 + j
      do k = 1, 9, 1
        ar3(i,j,k) = i*100 + j*10 + k
      end do
    end do
  end do

  do i = 1, 10, 1
    do j = -7, 3, 1
      do k = -15, -5, 1
        ar4(i,j,k) = i*100 + (j+8)*10 + (k+16)
      end do
    end do
  end do

  ar1(1) = 11  !BP1
  return
end

program testprog
  call sub
end
