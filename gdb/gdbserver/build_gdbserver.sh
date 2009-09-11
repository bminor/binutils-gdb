#Bash script to build gdbserver
export CFLAGS="-mA7 -static -O0"
export LDFLAGS="-mA7"
export CC=arc-linux-uclibc-gcc
./configure --host=i386-redhat-linux-gnu --target=arc-linux-uclibc
make
