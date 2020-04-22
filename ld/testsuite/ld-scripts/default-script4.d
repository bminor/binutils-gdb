# source: default-script.s
# ld: --default-script default-script.t -defsym _START=0x800
# nm: -n
# skip: x86_64-*-mingw* x86_64-*-cygwin
# Skipped on Mingw64 and Cygwin because the image base defaults to 0x100000000

#...
0*800 . _START
#...
0*800 T text
#pass
