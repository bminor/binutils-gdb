#name: AVR, check elf link-relax header flag is clear.
#as: -mmcu=avrxmega2
#readelf: -h
#source: link-relax-elf-flag.s
#target: avr-*-*

ELF Header:
#...
  Flags:                             0x66, avr:102
#...