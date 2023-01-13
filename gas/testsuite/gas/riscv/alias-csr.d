#source: alias-csr.s
#as: -march=rv64if
#objdump: -dr

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <alias_csr>:
[ 	]+0:[ 	]+003022f3[ 	]+frcsr[ 	]+t0
[ 	]+4:[ 	]+003392f3[ 	]+fscsr[ 	]+t0,t2
[ 	]+8:[ 	]+00339073[ 	]+fscsr[ 	]+t2
[ 	]+c:[ 	]+002022f3[ 	]+frrm[ 	]+t0
[ 	]+10:[ 	]+002312f3[ 	]+fsrm[ 	]+t0,t1
[ 	]+14:[ 	]+00231073[ 	]+fsrm[ 	]+t1
[ 	]+18:[ 	]+002fd2f3[ 	]+fsrmi[ 	]+t0,31
[ 	]+1c:[ 	]+002fd073[ 	]+fsrmi[ 	]+zero,31
[ 	]+20:[ 	]+001022f3[ 	]+frflags[ 	]+t0
[ 	]+24:[ 	]+001312f3[ 	]+fsflags[ 	]+t0,t1
[ 	]+28:[ 	]+00131073[ 	]+fsflags[ 	]+t1
[ 	]+2c:[ 	]+001fd2f3[ 	]+fsflagsi[ 	]+t0,31
[ 	]+30:[ 	]+001fd073[ 	]+fsflagsi[ 	]+zero,31
