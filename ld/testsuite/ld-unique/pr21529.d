#ld: --oformat binary -e main
#objdump: -s -b binary

.*:     file format binary

Contents of section \.data:
 0000 .*
#pass
