#name: Unknown SHF_MASKOS value in section
#source: readelf-maskos.s
#notarget: [supports_gnu_osabi] msp430-*-elf visium-*-elf
#xfail: arm-*-elf
#readelf: -S --wide
# PR26722 for the arm-*-elf XFAIL

#...
  \[[ 0-9]+\] .data.retain_var.*WAo.*
#pass
