#as: -march=rv64i_zknh
#source: zknh-64.s
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+.*:[ 	]+.*[ 	]+sha256sig0[ 	]+a0,a0
[ 	]+.*:[ 	]+.*[ 	]+sha256sig1[ 	]+a0,a0
[ 	]+.*:[ 	]+.*[ 	]+sha256sum0[ 	]+a0,a0
[ 	]+.*:[ 	]+.*[ 	]+sha256sum1[ 	]+a0,a0
[ 	]+.*:[ 	]+.*[ 	]+sha512sig0[ 	]+a0,a0
[ 	]+.*:[ 	]+.*[ 	]+sha512sig1[ 	]+a0,a0
[ 	]+.*:[ 	]+.*[ 	]+sha512sum0[ 	]+a0,a0
[ 	]+.*:[ 	]+.*[ 	]+sha512sum1[ 	]+a0,a0
