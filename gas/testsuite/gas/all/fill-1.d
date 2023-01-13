#objdump: -s -j .data -j "\$DATA\$"
#name: fill test with forward labels
#notarget: tic4x-*-* tic54x-*-*

.*: +file format .*

Contents of section (\.data|\$DATA\$):
 [^ ]* 0a0a0d0d 0b0b0c0c .*
