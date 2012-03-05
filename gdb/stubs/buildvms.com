$! Command to build the gdb stub
$cc /debug/noopt /pointer=64 gdbstub +sys$library:sys$lib_c.tlb/lib
$ link/notraceback/sysexe/map=gdbstub.map/full/share=gdbstub.exe gdbstub,sys$inp
ut/opt
$deck
cluster=gdbzero
collect=gdbzero, XFER_PSECT
$eod
$ search /nowarnings gdbstub.map "DECC$"
$! Example of use.
$ DEFINE /nolog LIB$DEBUG sys$login:gdbstub.exe
