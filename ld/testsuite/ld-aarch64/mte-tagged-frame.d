#source: mte-tagged-frame-foo.s
#source: mte-tagged-frame-bar.s
#ld: -shared
#objdump: -WF

#...
Contents of the .eh_frame section:


0+0 0+10 0+0 CIE "zR" cf=4 df=-8 ra=30
 +LOC +CFA +
0+0 sp\+0 +

0+14 0+20 0+18 FDE cie=0+0 pc=0+17c..0+188
 +LOC +CFA +x19 +x20 +
0+17c sp\+0 +u +u +
0+180 sp\+32 +c-32  c-16 +
0+184 sp\+0 +u +u +

0+38 0+14 0+0 CIE "zRG" cf=4 df=-8 ra=30
 +LOC +CFA +
0+0 sp\+0 +

0+50 0+1c 0+1c FDE cie=0+38 pc=0+188..0+194
 +LOC +CFA +x19 +x20 +
0+188 sp\+0 +u +u +
0+18c sp\+144 +c-144 c-136 +
0+190 sp\+0 +u +u +
#...
