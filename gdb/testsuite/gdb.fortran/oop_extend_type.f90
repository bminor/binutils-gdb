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

module testmod
    implicit none
    type :: point
        real :: coo(3)
    end type

    type, extends(point) :: waypoint
        real :: angle
    end type

end module

program testprog
    use testmod
    implicit none

    logical l
    type(waypoint) :: wp
    type(waypoint), allocatable :: wp_vla(:)

    l = allocated(wp_vla)
    allocate(wp_vla(3))               ! Before vla allocation

    l = allocated(wp_vla)             ! After vla allocation
    wp%angle = 100.00
    wp%point%coo(:) = 1.00
    wp%point%coo(2) = 2.00

    wp_vla(1)%angle = 101.00
    wp_vla(1)%point%coo(:) = 10.00
    wp_vla(1)%point%coo(2) = 12.00
    wp_vla(2)%angle = 102.00
    wp_vla(2)%point%coo(:) = 20.00
    wp_vla(2)%point%coo(2) = 22.00
    wp_vla(3)%angle = 103.00
    wp_vla(3)%point%coo(:) = 30.00
    wp_vla(3)%point%coo(2) = 32.00

    print *, wp, wp_vla               ! After value assignment

end program
