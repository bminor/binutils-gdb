@echo off
if "%1" == "h8/300" goto ok
echo Specify one of [ h8/300 ] on command line
goto exit

:ok
cd gcc
call configure %1
cd ../gas
call configure %1
cd ../bfd
call configure %1
cd ../binutils
call configure %1
cd ../ld
call configure %1
cd ../libiberty
call configure %1
cd ..

:exit
