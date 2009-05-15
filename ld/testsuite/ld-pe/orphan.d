#source: orphana.s
#source: orphanb.s
#source: orphand.s
#source: orphane.s
#ld: --file-align 1 --section-align 1
#objdump: -h --wide

#...
 +0 +\.text .*
 +1 +\.foo +0+8 .*
 +2 +\.foo +0+8 .*
 +3 +\.idata .*
#pass
