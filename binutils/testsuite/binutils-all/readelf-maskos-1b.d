#name: -t (section details) for unknown SHF_MASKOS value in section
#source: readelf-maskos.s
#notarget: [supports_gnu_osabi] msp430-*-elf visium-*-elf
#xfail: arm-*-elf
#readelf: -S -t --wide
# PR26722 for the arm-*-elf XFAIL

#...
  \[[ 0-9]+\] .data.retain_var
       PROGBITS +0+ +[0-9a-f]+ +[0-9a-f]+ +[0-9a-f]+ +0 +0 +(1|2|4|8)
       \[00200003\]: WRITE, ALLOC, OS \(00200000\)
#pass
