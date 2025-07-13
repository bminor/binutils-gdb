#name: SFrame relocatable link - discarded section (PR ld/33127)
#source: ../ld-elf/eh-group1.s
#source: ../ld-elf/eh-group2.s
#as: --gsframe
#ld: -r
#readelf: -rW

#failif
#...
[0-9a-f]+ +[0-9a-f]+ +R_X86_64_NONE *0?
#...
