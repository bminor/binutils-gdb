#source: data.s
#ld: -T data.t
#objdump: -s -j .text

.*:     file format .*

Contents of section .text:
 1000 (0410)?0000(1004)? (0020)?0000(2000)? .*
#pass
