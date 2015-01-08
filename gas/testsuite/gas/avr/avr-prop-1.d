#name: AVR '.avr.prop' test 1
#as: -mmcu=avrxmega2 -mlink-relax
#objdump: -P avr-prop
#source: avr-prop-1.s
#target: avr-*-*

.*:     file format elf32-avr

Contents of `\.avr\.prop' section:

  Version: 1
  Flags:   0

   0 ORG @ \.text\.1 \+ 0x000020 \(0x000020\)
   1 ORG @ \.text\.1 \+ 0x000044 \(0x000044\)
   2 ORG @ \.text\.2 \+ 0x000020 \(0x000020\)
   3 ALIGN @ \.text\.2 \+ 0x000020 \(0x000020\)
    Align: 0x000004
   4 ALIGN @ \.text\.2 \+ 0x000030 \(0x000030\)
    Align: 0x000004
   5 ORG @ \.text\.2 \+ 0x000200 \(0x000200\)
   6 ALIGN @ \.text\.2 \+ 0x000200 \(0x000200\)
    Align: 0x000004
   7 ALIGN @ \.text\.3 \+ 0x000100 \(0x000100\)
    Align: 0x000008

