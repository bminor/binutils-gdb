@echo off
if "%1" == "go32" goto h8300
if "%1" == "h8/300" goto h8300
echo Specify one of [ go32 h8/300 ] on command line
goto exit

:go32
echo Configuring gas for go32
copy config\i386.md md
copy config\out-i386.c aux-output.c
copy config\tm-i386gas.h tm.h
copy config\float.h-ieee float.h
copy config\xm-i386.h config.h
copy Makefile.dos Makefile
goto exit

:h8300
echo Configuring gas for H8/300
copy config\ho-go32.h host.h
copy config\tc-h8300.c targ-cpu.c
copy config\tc-h8300.h targ-cpu.h
copy config\te-generic.h targ-env.h
copy config\objcoffbfd.h obj-format.h
copy config\objcoffbfd.c obj-format.c
copy config\atof-ieee.c atof-targ.c
copy Makefile.dos Makefile

:exit
