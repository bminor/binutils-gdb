#objdump: -s -j .data -j "\$DATA\$"
#name: fill test with forward labels

.*: +file format .*

Contents of section (\.data|\$DATA\$):
 [^ ]* 0a0a0d0d 0b0b0c0c .*
