#as: --gdwarf-5
#name: Check line table is produced with .nops
#readelf: -wL

#...
Contents of the .debug_line section:

CU: .*
File name.*
#...
.*[ 	]+[1-8][ 	]+0.*
#pass
