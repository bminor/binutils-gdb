#as: -march=rv32i_zknh
#source: zknh-32.s
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+.*:[ 	]+.*[ 	]+sha256sig0[ 	]+a0,a0
[ 	]+.*:[ 	]+.*[ 	]+sha256sig1[ 	]+a0,a0
[ 	]+.*:[ 	]+.*[ 	]+sha256sum0[ 	]+a0,a0
[ 	]+.*:[ 	]+.*[ 	]+sha256sum1[ 	]+a0,a0
[ 	]+.*:[ 	]+.*[ 	]+sha512sig0h[ 	]+a0,a1,a2
[ 	]+.*:[ 	]+.*[ 	]+sha512sig0l[ 	]+a0,a1,a2
[ 	]+.*:[ 	]+.*[ 	]+sha512sig1h[ 	]+a0,a1,a2
[ 	]+.*:[ 	]+.*[ 	]+sha512sig1l[ 	]+a0,a1,a2
[ 	]+.*:[ 	]+.*[ 	]+sha512sum0r[ 	]+a0,a1,a2
[ 	]+.*:[ 	]+.*[ 	]+sha512sum1r[ 	]+a0,a1,a2
