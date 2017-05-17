#name: --gc-sections with relocations in debug section
#source: pr20882a.s
#source: pr20882b.s
#source: pr20882c.s
#as: -gdwarf-sections
#ld: --gc-sections -e main
#readelf: -x .debug_info

#...
 +0x0+ [0-9a-f ]+ 28 +.+\(
