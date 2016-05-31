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

program TestEntryPoint

  call foo(1,2,3,4)
  call bar(4,5,6,7)
  call tim(1)

end program TestEntryPoint

  subroutine bar(I,J,K,I1)
    INTEGER I,J,K,L,I1
    INTEGER A
    REAL    C

    A = 0
    C = 0.0

    A = I + K + I1
    goto 1000

    entry foo(J,K,L,I1)
    A = J + K + L + I1

200 C = J
    goto 1000

    entry tim(J)
    goto 200

1000 A = C + 1
     C = J * 1.5

    return
  end subroutine
