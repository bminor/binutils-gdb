@echo off
@echo Configuring binutils for H8/300, hosted on MS-DOS

copy ..\bfd\hosts\h-go32.h sysdep.h

copy makefile.dos makefile
