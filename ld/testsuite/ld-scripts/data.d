#source: data.s
#ld: -T data.t
#objdump: -s -j .text

.*:     file format .*

Contents of section .text:
 1000 (0410)?0000(1004)? (0810)?0000(1008)? +........ *
#pass
